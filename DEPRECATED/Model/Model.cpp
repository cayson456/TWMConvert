#include "precompiled.hpp"
#include "Model.hpp"


#include "Assimp/mesh.h"


Mesh::Mesh(uint64_t num_verts, uint64_t num_indicies) : mNumVertices(num_verts),
                                                        mNumIndices(num_indicies), 
                                                        mContainsNormals(false), 
                                                        mContainsUVs(false), 
                                                        mContainsTangents(false), 
                                                        mContainsBinormals(false), 
                                                        mContainsSkeleton(false)
{
    mPositions.reserve(static_cast<unsigned int>(num_verts) * 3);
    mIndices.reserve(static_cast<unsigned int>(num_indicies) * 3);

    mNormals.reserve(static_cast<unsigned int>(num_verts) * 3);
    mUVs.reserve(static_cast<unsigned int>(num_verts) * 2);
    mTangents.reserve(static_cast<unsigned int>(num_verts) * 3);
    mBinormals.reserve(static_cast<unsigned int>(num_verts) * 3);
}

Mesh::~Mesh()
{
}

void Mesh::CalculateVertexNormals()
{
    if (mPositions.empty())
        throw std::exception("Cannot calculate vertex normals, cause no vertices");
    
    mNormals.clear();
    mNormals.resize(mNumVertices, Vector3{0.0f,0.0f,0.0f});
 
        // For each face, calculate the normal and then add it to the vertex normal
    for (int i = 0; i < mIndices.size(); i += 3) {
        aiVector3D AB = mPositions[mIndices[i+1]] - mPositions[mIndices[i]];
        aiVector3D AC = mPositions[mIndices[i+2]] - mPositions[mIndices[i]];
 
        aiVector3D partial_normal = AB.Cross(AC);
 
        mNormals[mIndices[i]] += partial_normal;
        mNormals[mIndices[i+1]] += partial_normal;
        mNormals[mIndices[i+2]] += partial_normal;
    }
 
    for (auto n : mNormals)
        n.Normalize();
    
    mContainsNormals = true;
}
