#include "precompiled.hpp"

#include "FileWriter/FileWriter.hpp"

#include <Assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/mesh.h>
#include <Assimp/postprocess.h>
#include <Assimp/vector2.h>
#include <Assimp/vector3.h>

const uint32_t FILE_LAYOUT_VERSION = 3;

int main(int argc, char* argv[])
{
    FileWriter writer;

    if (argc != 3) {
        std::cout << "Arguments are invalid. " 
                  << "Should be \"twmconvert.exe (input file) (output filename)\""
                  << "Example: twmconvert.exe model.obj model.twm"
                  << std::endl;

        return -1;
    }

    std::filesystem::path filepath(argv[1]);

    Assimp::Importer importer;
    importer.SetPropertyFloat("PP_GSN_MAX_SMOOTHING_ANGLE", 90);

    const aiScene* scene = importer.ReadFile(filepath.string(), aiProcess_Triangulate
                                                              | aiProcess_GenSmoothNormals
                                                              | aiProcess_FlipUVs
                                                              | aiProcess_JoinIdenticalVertices
                                                              | aiProcess_FixInfacingNormals
                                                              | aiProcess_CalcTangentSpace
                                                              | aiProcess_ValidateDataStructure
                                                              | aiProcess_FindInvalidData
                                                              | aiProcess_FindDegenerates
                                                              | aiProcess_GenUVCoords);

    if (!scene) {
        std::cout << "Error opening file: " << filepath << " - " << importer.GetErrorString() << std::endl;
        return false;
    }


    // Center and Resize Mesh
    /////////////////////////////////////////////////////////

    aiVector3D min = { FLT_MAX, FLT_MAX, FLT_MAX };
    aiVector3D max = { FLT_MIN, FLT_MIN, FLT_MIN };

    // Find the bounding box and center
    for (size_t i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];

        for (size_t j = 0; j < mesh->mNumVertices; ++j) {
            min.x = std::min(min.x, mesh->mVertices[j].x);
            min.y = std::min(min.y, mesh->mVertices[j].y);
            min.z = std::min(min.z, mesh->mVertices[j].z);

            max.x = std::max(max.x, mesh->mVertices[j].x);
            max.y = std::max(max.y, mesh->mVertices[j].y);
            max.z = std::max(max.z, mesh->mVertices[j].z);
        }
    }

    aiVector3D center = (min + max) / 2.0f;
    aiVector3D size = max - min;

    float biggest_dimension = std::max(std::max(size.x, size.y), size.z);

    std::cout << "Minimum: " << min.x << ", " << min.y << ", " << min.z << std::endl;
    std::cout << "Maximum: " << max.x << ", " << max.y << ", " << max.z << std::endl;
    std::cout << "Center: " << center.x << ", " << center.y << ", " << center.z << std::endl;
    std::cout << "Size: " << size.x << ", " << size.y << ", " << size.z << std::endl << std::endl;

    std::cout << "Centering and Scaling... " << std::endl;

    for (size_t i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];

        for (size_t j = 0; j < mesh->mNumVertices; ++j) {
            mesh->mVertices[j] -= center;
            mesh->mVertices[j].x /= biggest_dimension;
            mesh->mVertices[j].y /= biggest_dimension;
            mesh->mVertices[j].z /= biggest_dimension;
        }
    }

    // Find the new bounding box and center
    aiVector3D new_min = { FLT_MAX, FLT_MAX, FLT_MAX };
    aiVector3D new_max = { FLT_MIN, FLT_MIN, FLT_MIN };

    for (size_t i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];

        for (size_t j = 0; j < mesh->mNumVertices; ++j) {
            new_min.x = std::min(new_min.x, mesh->mVertices[j].x);
            new_min.y = std::min(new_min.y, mesh->mVertices[j].y);
            new_min.z = std::min(new_min.z, mesh->mVertices[j].z);

            new_max.x = std::max(new_max.x, mesh->mVertices[j].x);
            new_max.y = std::max(new_max.y, mesh->mVertices[j].y);
            new_max.z = std::max(new_max.z, mesh->mVertices[j].z);
        }
    }

    aiVector3D new_center = (new_min + new_max) / 2.0f;
    aiVector3D new_size = new_max - new_min;

    std::cout << "New Minimum: " << new_min.x << ", " << new_min.y << ", " << new_min.z << std::endl;
    std::cout << "New Maximum: " << new_max.x << ", " << new_max.y << ", " << new_max.z << std::endl;
    std::cout << "New Center: " << new_center.x << ", " << new_center.y << ", " << new_center.z << std::endl;
    std::cout << "New Size: " << new_size.x << ", " << new_size.y << ", " << new_size.z << std::endl << std::endl;

    ///////////////////////////////////////////////////////////////////////////////////////////////
 
   // for (size_t i = 0; i < scene->mNumMeshes; ++i) {
   //     aiMesh* mesh = scene->mMeshes[i];
   //
   //     for (size_t j = 0; j < mesh->mNumVertices; ++j) {
   //         std::cout << "Position: " << mesh->mVertices[j].x << ", " << mesh->mVertices[j].y << ", " << mesh->mVertices[j].z << std::endl;
   //         std::cout << "Normal: " << mesh->mNormals[j].x << ", " << mesh->mNormals[j].y << ", " << mesh->mNormals[j].z << std::endl;
   //         std::cout << "UV: " << mesh->mTextureCoords[0][j].x << ", " << mesh->mTextureCoords[0][j].y  << std::endl;
   //     }
   // }

    // 00-03: ".twm" in characters
    writer.Write(".twm", 4);

    // 04-07: File Layout Release Version Number
    writer.Write(FILE_LAYOUT_VERSION, sizeof(uint32_t));

    // 08-11: # of Meshes (uint32_t)
    writer.Write(static_cast<uint32_t>(scene->mNumMeshes), sizeof(uint32_t));

    // 12: Contains Skeleton
    writer.Write(scene->hasSkeletons(), sizeof(bool));

    // 13-15: Reserved
    writer.WriteBlankBytes(3);

    // IF CONTAINS SKELETON
    if (scene->hasSkeletons()) {
        // TODO: Implement Skeleton Data
    }

    // For each mesh:
    for (size_t i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[i];

        // 0-3: # of Vertices (uint32_t)
        writer.Write(mesh->mNumVertices, sizeof(uint32_t));

        // 04: Contains Normals
        writer.Write(mesh->HasNormals(), sizeof(bool));
        // 05: Contains UVs
        writer.Write(mesh->HasTextureCoords(0), sizeof(bool));
        // 06: Contains Tangents
        writer.Write(mesh->HasTangentsAndBitangents(), sizeof(bool));
        // 07: Contains Binormals
        writer.Write(mesh->HasTangentsAndBitangents(), sizeof(bool));
        // 08: Contains Deformer
        writer.Write(false, sizeof(bool));

        // 09-11: Reserved
        writer.WriteBlankBytes(3);
    
        // All Vertices
        writer.Write(*mesh->mVertices, sizeof(aiVector3D) * mesh->mNumVertices);
        
        // ALl Normals
        if (mesh->HasNormals())
            writer.Write(*mesh->mNormals, sizeof(aiVector3D) * mesh->mNumVertices);
        
        // ALl UVs
        if (mesh->HasTextureCoords(0)) 
            writer.Write(*mesh->mTextureCoords[0], sizeof(aiVector2D) * mesh->mNumVertices);

        // ALl Tangents
        if (mesh->HasTangentsAndBitangents()) {
            writer.Write(*mesh->mTangents, sizeof(aiVector3D) * mesh->mNumVertices);
            writer.Write(*mesh->mBitangents, sizeof(aiVector3D) * mesh->mNumVertices);
        }   

        // 0-3: # of Indices (uint32_t)
        writer.Write(static_cast<uint32_t>(mesh->mNumFaces * 3), sizeof(uint32_t));

        // All Indices (we expect triangles)
        for (size_t j = 0; j < mesh->mNumFaces; ++j) {

        //    std::cout << mesh->mFaces[j].mIndices[0] << ", " << mesh->mFaces[j].mIndices[1] << ", " << mesh->mFaces[j].mIndices[2] << std::endl;

            writer.Write(mesh->mFaces[j].mIndices[0], sizeof(uint32_t));
            writer.Write(mesh->mFaces[j].mIndices[1], sizeof(uint32_t));
            writer.Write(mesh->mFaces[j].mIndices[2], sizeof(uint32_t));
        }
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