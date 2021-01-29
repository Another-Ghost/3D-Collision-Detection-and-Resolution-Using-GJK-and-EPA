/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLMesh.h"
#include "OGLTexture.h"

#include "../../Common/SimpleFont.h"
#include "../../Common/TextureLoader.h"

#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/Matrix4.h"

#include "../../Common/MeshGeometry.h"

#ifdef _WIN32
#include "../../Common/Win32Window.h"

#include "KHR\khrplatform.h"
#include "glad\glad.h"

#include "GL/GL.h"
#include "KHR/WGLext.h"

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
#endif

using namespace NCL;
using namespace NCL::Rendering;

#ifdef OPENGL_DEBUGGING
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
#endif;

OGLRenderer::OGLRenderer(Window& w) : RendererBase(w)	{
	initState = false;
#ifdef _WIN32
	InitWithWin32(w);
#endif
	boundMesh	= nullptr;
	boundShader = nullptr;

	currentWidth	= (int)w.GetScreenSize().x;
	currentHeight	= (int)w.GetScreenSize().y;

	if (initState) {
		TextureLoader::RegisterAPILoadFunction(OGLTexture::RGBATextureFromFilename);

		font = new SimpleFont("PressStart2P.fnt", "PressStart2P.png");

		OGLTexture* t = (OGLTexture*)font->GetTexture();

		if (t) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, t->GetObjectID());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		debugShader = new OGLShader("debugVert.glsl", "debugFrag.glsl");
	}

	forceValidDebugState = false;

	debugLinesMesh		= new OGLMesh();
	debugTextMesh		= new OGLMesh();


	debugLinesMesh->SetVertexPositions(std::vector<Vector3>(5000, Vector3()));
	debugLinesMesh->SetVertexColours(std::vector<Vector4>(5000, Vector3()));

	debugTextMesh->SetVertexPositions(std::vector<Vector3>(5000, Vector3()));
	debugTextMesh->SetVertexColours(std::vector<Vector4>(5000, Vector3()));
	debugTextMesh->SetVertexTextureCoords(std::vector<Vector2>(5000, Vector3()));

	debugTextMesh->UploadToGPU();
	debugLinesMesh->UploadToGPU();

	debugLinesMesh->SetPrimitiveType(GeometryPrimitive::Lines);
}

OGLRenderer::~OGLRenderer()	{
	delete font;
	delete debugShader;

#ifdef _WIN32
	DestroyWithWin32();
#endif
}

void OGLRenderer::OnWindowResize(int w, int h)	 {
	currentWidth	= w;
	currentHeight	= h;
	glViewport(0, 0, currentWidth, currentHeight);
}

void OGLRenderer::BeginFrame()		{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(nullptr);
	BindMesh(nullptr);
}

void OGLRenderer::RenderFrame()		{

}

void OGLRenderer::EndFrame()		{
	DrawDebugData();
}

void OGLRenderer::SwapBuffers()   {
	::SwapBuffers(deviceContext);
}

void OGLRenderer::BindShader(ShaderBase*s) {
	if (!s) {
		glUseProgram(0);
		boundShader = nullptr;
	}
	else if (OGLShader* oglShader = dynamic_cast<OGLShader*>(s)) {
		glUseProgram(oglShader->programID);
		boundShader = oglShader;
	}
	else {
		std::cout << __FUNCTION__ << " has received invalid shader?!" << std::endl;
		boundShader = nullptr;
	}
}

void OGLRenderer::BindMesh(MeshGeometry*m) {
	if (!m) {
		glBindVertexArray(0);
		boundMesh = nullptr;
	}
	else if (OGLMesh* oglMesh = dynamic_cast<OGLMesh*>(m)) {
		if (oglMesh->GetVAO() == 0) {
			std::cout << __FUNCTION__ << " has received invalid mesh?!" << std::endl;
		}
		glBindVertexArray(oglMesh->GetVAO());
		boundMesh = oglMesh;
	}
	else {
		std::cout << __FUNCTION__ << " has received invalid mesh?!" << std::endl;
		boundMesh = nullptr;
	}
}

void OGLRenderer::DrawBoundMesh(int subLayer, int numInstances) {
	if (!boundMesh) {
		std::cout << __FUNCTION__ << " has been called without a bound mesh!" << std::endl;
		return;
	}
	if (!boundShader) {
		std::cout << __FUNCTION__ << " has been called without a bound shader!" << std::endl;
		return;
	}
	GLuint	mode	= 0;
	int		count	= 0;
	int		offset	= 0;

	if (boundMesh->GetSubMeshCount() == 0) {
		if (boundMesh->GetIndexCount() > 0) {
			count = boundMesh->GetIndexCount();
		}
		else{
			count = boundMesh->GetVertexCount();
		}
	}
	else {
		const SubMesh* m = boundMesh->GetSubMesh(subLayer);
		offset = m->start;
		count  = m->count;
	}

	switch (boundMesh->GetPrimitiveType()) {
		case GeometryPrimitive::Triangles:		mode = GL_TRIANGLES;		break;
		case GeometryPrimitive::Points:			mode = GL_POINTS;			break;
		case GeometryPrimitive::Lines:			mode = GL_LINES;			break;
		case GeometryPrimitive::TriangleFan:	mode = GL_TRIANGLE_FAN;		break;
		case GeometryPrimitive::TriangleStrip:	mode = GL_TRIANGLE_STRIP;	break;
		case GeometryPrimitive::Patches:		mode = GL_PATCHES;			break;
	}

	if (boundMesh->GetIndexCount() > 0) {
		glDrawElements(mode, count, GL_UNSIGNED_INT, (const GLvoid*)(offset * sizeof(unsigned int)));
	}
	else {
		glDrawArrays(mode, 0, count);
	}
}

void OGLRenderer::BindTextureToShader(const TextureBase*t, const std::string& uniform, int texUnit) const{
	GLint texID = 0;

	if (!boundShader) {
		std::cout << __FUNCTION__ << " has been called without a bound shader!" << std::endl;
		return;//Debug message time!
	}
	
	GLuint slot = glGetUniformLocation(boundShader->programID, uniform.c_str());

	if (slot < 0) {
		return;
	}

	if (const OGLTexture* oglTexture = dynamic_cast<const OGLTexture*>(t)) {
		texID = oglTexture->GetObjectID();
	}

	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, texID);

	glUniform1i(slot, texUnit);
}

void OGLRenderer::DrawString(const std::string& text, const Vector2&pos, const Vector4& colour, float size) {
	DebugString s;
	s.colour	= colour;
	s.pos		= pos;
	s.size		= size;
	s.text		= text;
	debugStrings.emplace_back(s);
}

void OGLRenderer::DrawLine(const Vector3& start, const Vector3& end, const Vector4& colour) {
	DebugLine l;
	l.start		= start;
	l.end		= end;
	l.colour	= colour;
	debugLines.emplace_back(l);
}

Matrix4 OGLRenderer::SetupDebugLineMatrix() const {
	return Matrix4();
}
Matrix4 OGLRenderer::SetupDebugStringMatrix()const {
	return Matrix4();
}

void OGLRenderer::DrawDebugData() {
	if (debugStrings.empty() && debugLines.empty()) {
		return; //don't mess with OGL state if there's no point!
	}
	BindShader(debugShader);

	if (forceValidDebugState) {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	int matLocation		= glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	Matrix4 pMat;

	BindTextureToShader(font->GetTexture(), "mainTex", 0);

	GLuint texSlot = glGetUniformLocation(boundShader->programID, "useTexture");

	if (debugLines.size() > 0) {
		pMat = SetupDebugLineMatrix();
		glUniformMatrix4fv(matLocation, 1, false, pMat.array);
		glUniform1i(texSlot, 0);
		DrawDebugLines();
	}

	if (debugStrings.size() > 0) {
		pMat = SetupDebugStringMatrix();
		glUniformMatrix4fv(matLocation, 1, false, pMat.array);
		glUniform1i(texSlot, 1);
		DrawDebugStrings();
	}

	if (forceValidDebugState) {
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

void OGLRenderer::DrawDebugStrings() {
	vector<Vector3> vertPos;
	vector<Vector2> vertTex;
	vector<Vector4> vertColours;

	for (DebugString&s : debugStrings) {
		font->BuildVerticesForString(s.text, s.pos, s.colour, s.size, vertPos, vertTex, vertColours);
	}

	debugTextMesh->SetVertexPositions(vertPos);
	debugTextMesh->SetVertexTextureCoords(vertTex);
	debugTextMesh->SetVertexColours(vertColours);
	debugTextMesh->UpdateGPUBuffers(0, vertPos.size());

	BindMesh(debugTextMesh);
	DrawBoundMesh();

	debugStrings.clear();
}

void OGLRenderer::DrawDebugLines() {
	vector<Vector3> vertPos;
	vector<Vector4> vertCol;

	for (DebugLine&s : debugLines) {
		vertPos.emplace_back(s.start);
		vertPos.emplace_back(s.end);

		vertCol.emplace_back(s.colour);
		vertCol.emplace_back(s.colour);
	}

	debugLinesMesh->SetVertexPositions(vertPos);
	debugLinesMesh->SetVertexColours(vertCol);
	debugLinesMesh->UpdateGPUBuffers(0, vertPos.size());

	BindMesh(debugLinesMesh);
	DrawBoundMesh();

	debugLines.clear();
}

#ifdef _WIN32
void OGLRenderer::InitWithWin32(Window& w) {
	Win32Code::Win32Window* realWindow = (Win32Code::Win32Window*)&w;

	if (!(deviceContext = GetDC(realWindow->GetHandle()))) {
		std::cout << __FUNCTION__ << " Failed to create window!" << std::endl;
		return;
	}

	//A pixel format descriptor is a struct that tells the Windows OS what type of front / back buffers we want to create etc
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   //It must be double buffered, it must support OGL(!), and it must allow us to draw to it...
	pfd.iPixelType	= PFD_TYPE_RGBA;	//We want our front / back buffer to have 4 channels!
	pfd.cColorBits	= 32;				//4 channels of 8 bits each!
	pfd.cDepthBits	= 24;				//24 bit depth buffer
	pfd.cStencilBits = 8;				//plus an 8 bit stencil buffer
	pfd.iLayerType	= PFD_MAIN_PLANE;

	GLuint		PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(deviceContext, &pfd))) {	// Did Windows Find A Matching Pixel Format for our PFD?
		std::cout << __FUNCTION__ << " Failed to choose a pixel format!" << std::endl;
		return;
	}

	if (!SetPixelFormat(deviceContext, PixelFormat, &pfd)) {		// Are We Able To Set The Pixel Format?
		std::cout << __FUNCTION__ << " Failed to set a pixel format!" << std::endl;
		return;
	}

	HGLRC		tempContext;		//We need a temporary OpenGL context to check for OpenGL 3.2 compatibility...stupid!!!
	if (!(tempContext = wglCreateContext(deviceContext))) {	// Are We Able To get the temporary context?
		std::cout << __FUNCTION__ <<"  Cannot create a temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if (!wglMakeCurrent(deviceContext, tempContext)) {	// Try To Activate The Rendering Context
		std::cout << __FUNCTION__ << " Cannot set temporary context!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}
	if (!gladLoadGL()) {
		std::cout << __FUNCTION__ << " Cannot initialise GLAD!" << std::endl;	//It's all gone wrong!
		return;
	}
	//Now we have a temporary context, we can find out if we support OGL 4.x
	char* ver = (char*)glGetString(GL_VERSION); // ver must equal "4.1.0" (or greater!)
	int major = ver[0] - '0';		//casts the 'correct' major version integer from our version string
	int minor = ver[2] - '0';		//casts the 'correct' minor version integer from our version string

	if (major < 3) {					//Graphics hardware does not support OGL 4! Erk...
		std::cout << __FUNCTION__ << " Device does not support OpenGL 4.x!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}

	if (major == 4 && minor < 1) {	//Graphics hardware does not support ENOUGH of OGL 4! Erk...
		std::cout << __FUNCTION__ << " Device does not support OpenGL 4.1!" << std::endl;
		wglDeleteContext(tempContext);
		return;
	}
	//We do support OGL 4! Let's set it up...

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#ifdef OPENGL_DEBUGGING 
		| WGL_CONTEXT_DEBUG_BIT_ARB
#endif		//No deprecated stuff!! DIE DIE DIE glBegin!!!!
		,WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0					//That's enough attributes...
	};

	//Everywhere else in the Renderers, we use function pointers provided by GLEW...but we can't initialise GLEW yet! So we have to use the 'Wiggle' API
	//to get a pointer to the function that will create our OpenGL 3.2 context...
	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	renderContext = wglCreateContextAttribsARB(deviceContext, 0, attribs);

	// Check for the context, and try to make it the current rendering context
	if (!renderContext || !wglMakeCurrent(deviceContext, renderContext)) {
		std::cout << __FUNCTION__ <<" Cannot set OpenGL 3 context!" << std::endl;	//It's all gone wrong!
		wglDeleteContext(renderContext);
		wglDeleteContext(tempContext);
		return;
	}

	wglDeleteContext(tempContext);	//We don't need the temporary context any more!

	std::cout << __FUNCTION__ << " Initialised OpenGL " << major << "." << minor << " rendering context" << std::endl;	//It's all gone wrong!

	glEnable(GL_FRAMEBUFFER_SRGB);

#ifdef OPENGL_DEBUGGING
	glDebugMessageCallback(DebugCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
#endif

	//If we get this far, everything's going well!
	initState = true; 

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

	w.SetRenderer(this);
}

void OGLRenderer::DestroyWithWin32() {
	wglDeleteContext(renderContext);
}

bool OGLRenderer::SetVerticalSync(VerticalSyncState s) {
	if (!wglSwapIntervalEXT) {
		return false;
	}
	GLuint state;

	switch (s) {
		case VerticalSyncState::VSync_OFF:		state =  0; break;
		case VerticalSyncState::VSync_ON:		state =  1; break;
		case VerticalSyncState::VSync_ADAPTIVE:	state = -1; break;
	}

	return wglSwapIntervalEXT(state);
}
#endif

#ifdef OPENGL_DEBUGGING
static void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	string sourceName;
	string typeName;
	string severityName;

	switch (source) {
	case GL_DEBUG_SOURCE_API_ARB: sourceName = "Source(OpenGL)"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: sourceName = "Source(Window System)"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: sourceName = "Source(Shader Compiler)"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: sourceName = "Source(Third Party)"; break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB: sourceName = "Source(Application)"; break;
	case GL_DEBUG_SOURCE_OTHER_ARB: sourceName = "Source(Other)"; break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR_ARB: typeName = "Type(Error)"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: typeName = "Type(Deprecated Behaviour)"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: typeName = "Type(Undefined Behaviour)"; break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB: typeName = "Type(Portability)"; break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB: typeName = "Type(Performance)"; break;
	case GL_DEBUG_TYPE_OTHER_ARB: typeName = "Type(Other)"; break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH_ARB: severityName = "Priority(High)"; break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: severityName = "Priority(Medium)"; break;
	case GL_DEBUG_SEVERITY_LOW_ARB: severityName = "Priority(Low)"; break;
	}

	std::cout << "OpenGL Debug Output: " + sourceName + ", " + typeName + ", " + severityName + ", " + string(message) << std::endl;
}
#endif