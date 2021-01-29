#pragma once
#ifdef _ORBIS
#include "../../Common/MeshGeometry.h"
#include "PS4MemoryAware.h"

#include <gnm.h>

#include <gnmx\context.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <gnm\dataformats.h>


namespace NCL {
	namespace PS4 {
		using namespace sce;
		class PS4Mesh :
			public NCL::MeshGeometry, public PS4MemoryAware
		{
			friend class PS4RendererBase;
		public:
			static PS4Mesh* GenerateTriangle();
			static PS4Mesh* GenerateQuad();
			static PS4Mesh* GenerateSinglePoint();

		protected:
			void	SubmitPreDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage);
			void	SubmitDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage);

			void	InitAttributeBuffer(sce::Gnm::Buffer &buffer, Gnm::DataFormat format, void*offset);

		protected:
			PS4Mesh();
			PS4Mesh(const std::string&filename);
			~PS4Mesh();

		protected:
			void UploadToGPU() override;

			//Gpu simply has a 4 byte alignment!
			struct MeshVertex
			{
				float position[3];
				float textureCoord[2];
				float normal[3];
				float tangent[3];
			};

			sce::Gnm::IndexSize		indexType;
			sce::Gnm::PrimitiveType primitiveType;



			int*		indexBuffer;
			MeshVertex*	vertexBuffer;

			int	vertexDataSize;
			int indexDataSize;

			sce::Gnm::Buffer*	attributeBuffers;
			int					attributeCount;
		};
	}
}
#endif
