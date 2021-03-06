#pragma once

#include "GeometrySpec.h"

#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include <vector>

#include "Filesystem.h"

BEGINNAMESPACE

namespace Importer {
	
    GeometrySpec* geometryFromMeshVec(const std::vector<aiMesh*>& subMeshes, const aiMatrix4x4& mat = aiMatrix4x4(), bool globalCoords = true);

	GeometrySpec* geometryByUUID(UUID uuid);

}
/*
Mesh* mesh_process(const aiMesh* mesh, const aiMatrix4x4& transformation, Package& package); //get mesh from within a scene 

void mesh_write(FILE* file, Mesh* mesh);
void mesh_setOffsets(Mesh* mesh);
uint32 mesh_getSize(const Mesh* mesh);
*/

ENDNAMESPACE
