#pragma once

class Mesh
{
public:
    Mesh() :  mNumVertices(0), mNumIndices(0), mContainsNormals(false), mContainsUVs(false), mContainsTangents(false), mContainsBinormals(false), mContainsSkeleton(false) {}
    Mesh(uint64_t num_verts, uint64_t num_indices);
    ~Mesh();
    
    uint64_t mNumVertices;
    uint64_t mNumIndices;

    std::vector<Vector3> mPositions;
    std::vector<uint64_t> mIndices;
    std::vector<Vector3> mNormals;
    std::vector<Vector2> mUVs;
    std::vector<Vector3> mTangents;
    std::vector<Vector3> mBinormals;

    void CalculateVertexNormals();
    
    bool mContainsNormals;
    bool mContainsUVs;
    bool mContainsTangents;
    bool mContainsBinormals;
    bool mContainsSkeleton;

//  bool m_containsDeformer;
//  std::vector<std::vector<BlendingWeight>> m_blendingWeights;
};

class Model
{
public:
    Model() : mContainsSkeleton(false) {}
    
    ~Model() {}

    std::vector<Mesh> mMeshes;

    bool mContainsSkeleton;

  //  Skeleton m_skeleton;
};