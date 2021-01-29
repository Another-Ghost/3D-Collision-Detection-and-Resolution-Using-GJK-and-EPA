#version 330 core

uniform samplerCube cubeTex;

in Vertex {
	vec3 viewDir;
} IN;

out vec4 fragColour;

void main(void)	{
	vec4 samp = texture(cubeTex,normalize(IN.viewDir));
	fragColour = pow(samp, vec4(2.2f));
}