#include <Mesh/ImportMesh.h>
#include <Material/ImportMaterial.h>

#include <Package/Package.h>
#include <Utilities/Assert.h>

#include <Utilities/Number.h>

#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/color4.h>
#include <assimp/Importer.hpp>

BEGINNAMESPACE

namespace Importer {

	using Mesh = Resource::Mesh;

#if defined _DEBUG
#	define CHECK( result, cond, message, ... ) \
	ASSERT(cond, message, __VA_ARGS__); \
	result = result && cond
#	else
#	define CHECK( result, cond, message ) result = result && cond
#endif

	namespace mesh_util {

		// all meshes in the meshes array will be combined
		Mesh* meshInitialize(aiMesh* meshes, const uint32 numSubMeshes) {
			if (numSubMeshes < 1) return nullptr;

			ASSERT(meshes[0].HasNormals(), "The mesh has no Normal informations, which are required");
			ASSERT(meshes[0].GetNumUVChannels() == 0 || meshes[0].HasTangentsAndBitangents(), "A mesh which contains texture coordinates should also contain (Bi)tangents");
			ASSERT(meshes[0].mNumVertices < std::numeric_limits<uint32>::max(), "Only support %d vertices.", std::numeric_limits<uint32>::max());
			ASSERT(Number::IsPowerOf2(meshes[0].mPrimitiveTypes) && meshes[0].mPrimitiveTypes <= AI_PRIMITIVE_TYPE_FOR_N_INDICES(4), "Primitives should have <=4 indices and only consist of one type per mesh");

			//----------------------------------
			//Check wheter the submeshes are compatible
			bool compatible = true;
			for (uint32 sub = 1; sub < numSubMeshes; ++sub) {
				CHECK(compatible, meshes[sub - 1].HasPositions() == meshes[sub].HasPositions(), "mesh %d and %d incompatible: Position", sub - 1, sub);
				CHECK(compatible, meshes[sub - 1].HasNormals() == meshes[sub].HasNormals(), "mesh %d and %d incompatible: Normal", sub - 1, sub);
				CHECK(compatible, meshes[sub - 1].HasTangentsAndBitangents() == meshes[sub].HasTangentsAndBitangents(), "mesh %d and %d incompatible: Tangent/Bitangents", sub - 1, sub);
				CHECK(compatible, meshes[sub - 1].GetNumColorChannels() == meshes[sub].GetNumColorChannels(), "mesh %d and %d incompatible: NumColorChannels", sub - 1, sub);
				CHECK(compatible, meshes[sub - 1].GetNumUVChannels() == meshes[sub].GetNumUVChannels(), "mesh %d and %d incompatible: UV-Channel", sub - 1, sub);
			}
			if (!compatible) return nullptr; //not compatible -> return nullptr
			//-----------------------------------

			//
			/// compute buffer size
			size_type memSize = sizeof(Mesh);
			//Count all vertices in this Mesh (combination of all submeshes)
			uint32 vertexCount = 0;
			uint32 primitiveCount = 0;
			for (uint32 i = 0; i < numSubMeshes; ++i) {
				vertexCount += meshes[i].mNumVertices;
				primitiveCount += meshes[i].mNumFaces;
			}
			//--
			
			uint32 primitiveType = meshFromAiPrimitive(meshes[0].mPrimitiveTypes);
			for (uint32 i = 0; i < numSubMeshes; ++i) {
				ASSERT(primitiveType == meshFromAiPrimitive(meshes[i].mPrimitiveTypes), "Each Mesh should only contain one type of Primitive!");
				memSize += meshes[i].mNumVertices * Resource::mesh::VertexStride(primitiveType, meshes[i].GetNumColorChannels(), meshes[i].GetNumUVChannels(), meshes[i].mNumUVComponents); //Storage for vertices
				memSize += meshes[i].mNumFaces * (vertexCount < 65535 ? sizeof(uint16) : sizeof(uint32)) * primitiveType;
			}
			
			union {
				Byte* meshAlloc;
				Mesh* mesh;
			};

			meshAlloc = new Byte[memSize]; //allocate memSize-bytes
			std::memset(meshAlloc, 0, memSize);

			//fill header
			mesh->header.numColorChannels = meshes[0].GetNumColorChannels();
			mesh->header.numPrimitives = primitiveCount;
			mesh->header.numSubMeshes = numSubMeshes;
			mesh->header.numUVChannels = meshes[0].GetNumUVChannels();
			mesh->header.numVertices = vertexCount;
			mesh->header.primitiveType = Resource::mesh::PrimitiveType(primitiveType);

			for (uint32 uvChannel = 0; uvChannel < meshes[0].GetNumUVChannels(); ++uvChannel) {
				mesh->header.uvChannelComponents[uvChannel] = meshes[0].mNumUVComponents[uvChannel];
			}

			//fill submesh header
			uint32 startVertex = 0;
			for (uint32 subMeshIdx = 0; subMeshIdx < numSubMeshes; ++subMeshIdx) {
				mesh->subHeader[subMeshIdx].startVertex = startVertex;
				mesh->subHeader[subMeshIdx].numIndices = meshes[subMeshIdx].mNumFaces;
				startVertex += meshes[subMeshIdx].mNumVertices;
			}

			return mesh; //return initialized mesh -> the data slots need to be filled
		}

		typedef struct {
			uint32 index;
			uint32 lastVertex;
			uint32 lastIndex;
		} MeshOffset;

		void meshCopy(Mesh* mesh, const aiMesh* _aiMesh, uint32 numMeshes, const aiMatrix4x4& transformation, bool globalCoords = true) {
			
			aiMatrix4x4 invertTranspTransform = transformation;
			invertTranspTransform.Inverse().Transpose();

			uint32 vertexIdx = 0;
			union {
				float32* vertices;
				void* vertexAsVoid;
			};
			vertexAsVoid = mesh->data.vertices;

			aiVector3D globalMinAABB(std::numeric_limits<float32>::max(), std::numeric_limits<float32>::max(), std::numeric_limits<float32>::max());
			aiVector3D globalMaxAABB(std::numeric_limits<float32>::min(), std::numeric_limits<float32>::min(), std::numeric_limits<float32>::min());

			for (uint32 subMesh = 0; subMesh < numMeshes; ++subMesh) {
				const aiMesh* aiMesh = (_aiMesh + subMesh);

				aiVector3D minAABB(std::numeric_limits<float32>::max(), std::numeric_limits<float32>::max(), std::numeric_limits<float32>::max());
				aiVector3D maxAABB(std::numeric_limits<float32>::min(), std::numeric_limits<float32>::min(), std::numeric_limits<float32>::min());
				//
				/// copy vertex data (position, normals, Color, TexCoords,...)
				for (uint32 vertIdx = 0; vertIdx < _aiMesh[subMesh].mNumVertices; ++vertIdx) {
					//?! Position data
					if ((mesh->header.vertexAttribs & VertexAttrib::Position) != 0) {
						aiVector3D position = _aiMesh[subMesh].mVertices[vertIdx];
						if (globalCoords)
							position *= transformation;
						vertices[vertexIdx++] = position.x;
						vertices[vertexIdx++] = position.y;
						vertices[vertexIdx++] = position.z;

						//update AABB
						if (position.x < minAABB.x) minAABB.x = position.x;
						if (position.y < minAABB.y) minAABB.y = position.y;
						if (position.z < minAABB.z) minAABB.z = position.z;
						if (position.x > maxAABB.x) maxAABB.x = position.x;
						if (position.y > maxAABB.y) maxAABB.y = position.y;
						if (position.z > maxAABB.z) maxAABB.z = position.z;
					}

					//?! Normal data
					if ((mesh->header.vertexAttribs & VertexAttrib::Normal) != 0) {
						aiVector3D normal = _aiMesh[subMesh].mNormals[vertIdx];
						if (globalCoords)
							normal *= invertTranspTransform;
						vertices[vertexIdx++] = normal.x;
						vertices[vertexIdx++] = normal.y;
						vertices[vertexIdx++] = normal.z;
					}

					//?! Color Data
					if ((mesh->header.vertexAttribs & VertexAttrib::Color) != 0) {
						for (uint32 colorChannel = 0; _aiMesh[subMesh].GetNumColorChannels(); ++colorChannel) {
							vertices[vertexIdx++] = _aiMesh[subMesh].mColors[vertIdx][colorChannel].r;
							vertices[vertexIdx++] = _aiMesh[subMesh].mColors[vertIdx][colorChannel].g;
							vertices[vertexIdx++] = _aiMesh[subMesh].mColors[vertIdx][colorChannel].b;
							vertices[vertexIdx++] = _aiMesh[subMesh].mColors[vertIdx][colorChannel].a;
						}
					}

					//?! Texture Data
					if ((mesh->header.vertexAttribs & VertexAttrib::Texture) != 0) {
						for (uint32 textureChannel = 0; _aiMesh[subMesh].GetNumColorChannels(); ++textureChannel) {
							vertices[vertexIdx++] = _aiMesh[subMesh].mTextureCoords[vertIdx][textureChannel].x;
							if (mesh->header.uvChannelComponents[textureChannel] > 1)
								vertices[vertexIdx++] = _aiMesh[subMesh].mTextureCoords[vertIdx][textureChannel].y;
							if (mesh->header.uvChannelComponents[textureChannel] > 2)
								vertices[vertexIdx++] = _aiMesh[subMesh].mTextureCoords[vertIdx][textureChannel].z;
						}
					}

					//?! Tangent-Bitangent Data
					if ((mesh->header.vertexAttribs & VertexAttrib::TangentBinormal) != 0) {
						aiVector3D tangent = _aiMesh[subMesh].mTangents[vertIdx];
						if (globalCoords)
							tangent *= invertTranspTransform;
						aiVector3D bitangent = _aiMesh[subMesh].mBitangents[vertIdx];
						if (globalCoords)
							bitangent *= invertTranspTransform;
						vertices[vertexIdx++] = tangent.x;
						vertices[vertexIdx++] = tangent.y;
						vertices[vertexIdx++] = tangent.z;
						vertices[vertexIdx++] = bitangent.x;
						vertices[vertexIdx++] = bitangent.y;
						vertices[vertexIdx++] = bitangent.z;
					}

					//?! Animation Data					
					// Copy Animation Data here...!
					//
				}

				//> insert AABB data
				mesh->data.bounding[subMesh + 1].aabb[0] = minAABB.x;
				mesh->data.bounding[subMesh + 1].aabb[1] = minAABB.y;
				mesh->data.bounding[subMesh + 1].aabb[2] = minAABB.z;
				mesh->data.bounding[subMesh + 1].aabb[3] = maxAABB.x;
				mesh->data.bounding[subMesh + 1].aabb[4] = maxAABB.y;
				mesh->data.bounding[subMesh + 1].aabb[5] = maxAABB.z;


				if(globalMinAABB.x < minAABB.x)
					globalMinAABB.x = minAABB.x;
				if (globalMinAABB.y < minAABB.y)
					globalMinAABB.y = minAABB.y;
				if (globalMinAABB.z < minAABB.z)
					globalMinAABB.z = minAABB.z;

				if (globalMaxAABB.x > minAABB.x)
					globalMaxAABB.x = maxAABB.x;
				if (globalMaxAABB.y > maxAABB.y)
					globalMaxAABB.y = maxAABB.y;
				if (globalMaxAABB.z > maxAABB.z)
					globalMaxAABB.z = maxAABB.z;

				union {
					uint16* indices16;
					uint32* indices32;
					void* indicesAsVoid;
				};
				indicesAsVoid = mesh->data.indices;

				//
				/// copy face data (indices)
				uint32 idxIdx = 0;
				for (uint32 faceIdx = 0; faceIdx < _aiMesh[subMesh].mNumFaces; ++faceIdx) {
					const aiFace* face = _aiMesh[subMesh].mFaces + faceIdx;
					for (uint32 indexIdx = 0; indexIdx < face->mNumIndices; ++indexIdx) {
						if (mesh->header.numVertices < 65535)
							indices16[idxIdx++] = uint16(face->mIndices[indexIdx]);
						else
							indices32[idxIdx++] = uint32(face->mIndices[indexIdx]);
					}
				}

				//
				/// copy material ref
				mesh->data.materials[subMesh] = Importer::materialByIndex(_aiMesh[subMesh].mMaterialIndex);
			}

			mesh->data.bounding[0].aabb[0] = globalMinAABB.x;
			mesh->data.bounding[0].aabb[1] = globalMinAABB.y;
			mesh->data.bounding[0].aabb[2] = globalMinAABB.z;
			mesh->data.bounding[0].aabb[3] = globalMaxAABB.x;
			mesh->data.bounding[0].aabb[4] = globalMaxAABB.y;
			mesh->data.bounding[0].aabb[5] = globalMaxAABB.z;
		}
	}

	void meshFromNode(const aiNode* node, const aiScene * scene, Meshes& outMeshes) {
		if (node->mNumMeshes > 0) {
			std::vector<aiMesh*> subMeshes(node->mNumMeshes);
			for (uint32 i = 0; i < node->mNumMeshes; ++i) {
				subMeshes.push_back(scene->mMeshes[node->mMeshes[i]]);
			}

			Mesh* mesh = mesh_util::meshInitialize(subMeshes[0], node->mNumMeshes);
			mesh_util::meshCopy(mesh, subMeshes[0], node->mNumMeshes, node->mTransformation);
			outMeshes.push_back(mesh);
		}
		for (uint32 i = 0; i < node->mNumChildren; ++i) {
			meshFromNode(node->mChildren[i], scene, outMeshes);
		}
	}

	Meshes meshAllFromScene(const aiScene * scene)
	{
		aiNode* node = scene->mRootNode;
		Meshes out;
		meshFromNode(node, scene, out);
		return out;
	}

	uint32 meshFromAiPrimitive(uint32 type)
	{
		uint32 flags = 0;
		flags |= (type & aiPrimitiveType_POINT ? Resource::mesh::PrimitiveType::Primitive_Point : 0);
		flags |= (type & aiPrimitiveType_LINE ? Resource::mesh::PrimitiveType::Primitive_Line : 0);
		flags |= (type & aiPrimitiveType_TRIANGLE ? Resource::mesh::PrimitiveType::Primitive_Triangle : 0);
		flags |= (type & AI_PRIMITIVE_TYPE_FOR_N_INDICES(4) ? Resource::mesh::PrimitiveType::Primitive_Quad : 0);
		return flags;
	}
	Meshes meshAllFromFile(const filesys::path & file)
	{
		return Meshes();
	}
}

ENDNAMESPACE

