#include "precompiled.hpp"

#include "Model/Model.hpp"
#include "FileWriter/FileWriter.hpp"
#include "OBJLoader/OBJLoader.hpp"


const uint32_t FILE_LAYOUT_VERSION = 0;

int main(int argc, char* argv[])
{
    Model model;
    FileWriter writer;

    std::cout << sizeof(Vector3) << std::endl;

    if (argc != 3) {
        std::cout << "Arguments are invalid. " 
                  << "Should be \"twmconvert.exe (input file) (output filename)\""
                  << "Example: twmconvert.exe model.obj model.twm"
                  << std::endl;

        return -1;
    }

    std::filesystem::path filepath(argv[1]);


    if (filepath.extension() == ".obj")
        if (OBJLoader::LoadModel(model, filepath) != 0)
            return -1;

    // TODO: Implement FBX and other fileformats

    // 00-03: ".twm" in characters
    writer.Write(".twm", 4);

    // 04-07: File Layout Release Version Number
    writer.Write(FILE_LAYOUT_VERSION, sizeof(uint32_t));

    // 08-11: # of Meshes (uint32_t)
    writer.Write(static_cast<uint32_t>(model.mMeshes.size()), sizeof(uint32_t));

    // 12: Contains Skeleton
    writer.Write(true, sizeof(bool));

    // 13-15: Reserved
    writer.WriteBlankBytes(3);

    // IF CONTAINS SKELETON
    if (model.mContainsSkeleton) {
        // TODO: Implement Skeleton Data
    }

    // For each mesh:
    for (const auto& mesh : model.mMeshes) {
        // 0-7: # of Vertices (uint64_t)
        writer.Write(mesh.mNumVertices, sizeof(uint64_t));

        // 08: Contains Normals
        writer.Write(mesh.mContainsNormals, sizeof(bool));
        // 09: Contains UVs
        writer.Write(mesh.mContainsUVs, sizeof(bool));
        // 10: Contains Tangents
        writer.Write(mesh.mContainsTangents, sizeof(bool));
        // 11: Contains Binormals
        writer.Write(mesh.mContainsBinormals, sizeof(bool));
        // 12: Contains Deformer
        writer.Write(false, sizeof(bool));

        // 13-15 Reserved
        writer.WriteBlankBytes(3);

        // All Vertices
        writer.Write(mesh.mPositions[0], sizeof(Vector3) * mesh.mPositions.size());
        
        // ALl Normals
        writer.Write(mesh.mNormals[0], sizeof(Vector3) * mesh.mNormals.size());
        
        // ALl UVs
        if (mesh.mContainsUVs)
            writer.Write(mesh.mUVs[0], sizeof(Vector2) * mesh.mUVs.size());
        
        // ALl Tangents
        if (mesh.mContainsTangents)
            writer.Write(mesh.mTangents[0], sizeof(Vector3) * mesh.mTangents.size());
        
        // ALl Binormals
        if (mesh.mContainsBinormals)
            writer.Write(mesh.mBinormals[0], sizeof(Vector3) * mesh.mBinormals.size());

        // 0-7: # of Indices (uint64_t)
        writer.Write(static_cast<uint64_t>(mesh.mIndices.size()), sizeof(uint64_t));

        // All Indices
        writer.Write(mesh.mIndices[0], sizeof(float) * mesh.mIndices.size());
    }


    // 0-3: # of Animations (uint32_t)
    writer.Write(0, sizeof(uint32_t));

    // Animation Data
    // TODO: Implement Animation Data
    

    // 0-15 End of File
    writer.Write(".twm END OF FILE", 16);
    
    writer.WriteToFile(argv[2]);

    std::cout << "Successfully formatted " << argv[2] << std::endl;

    return 0;
}