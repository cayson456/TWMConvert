#include "precompiled.hpp"
#include "OBJLoader.hpp"

int OBJLoader::LoadModel(Model& model, const std::filesystem::path &filepath)
{
    std::cout << "Opening file: " << filepath << std::endl;

    std::ifstream file_stream;
    file_stream.open(filepath, std::ifstream::in);

    if ((file_stream.rdstate() & std::ifstream::failbit) != 0) {
        std::cout << "Error opening OBJ: " << filepath << std::endl;
        return -1;
    }

    bool contains_vp = false;

    Mesh new_mesh;

    while (file_stream.good()) {
        std::string line;
        std::getline(file_stream, line);

            // Tokenize the line
        const std::regex whitespace("\\s+");
        std::sregex_token_iterator token_it(line.begin(), line.end(), whitespace, -1);
        std::sregex_token_iterator end;

        std::vector<std::string> tokens(token_it, end);

            // # - Comment
        if (tokens[0] == "#") {
            continue;
        }
            // v - Vertex
        else if (tokens[0] == "v") {
            new_mesh.mPositions.push_back(Vector3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
            ++new_mesh.mNumVertices;
        }
            // vt - Texture Coordinates
        else if (tokens[0] == "vt") {
            new_mesh.mUVs.push_back(Vector2(std::stof(tokens[1]), std::stof(tokens[2])));
            new_mesh.mContainsUVs = true;
        }
            // vn - Vertex Normals
        else if (tokens[0] == "vn") {
            new_mesh.mNormals.push_back(Vector3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
            new_mesh.mContainsNormals = true;
        }
            // vp - Parameter Space Vertices (unimplemented)
        else if (tokens[0] == "vp") {
            if (!contains_vp) {
                std::cout << "Parameter Space Vertices Not Implemented [vp]" << std::endl;
                contains_vp = true;
            }
            continue;
        }
            // f - Faces
        else if (tokens[0] == "f") {
                // Subtract 1 from each index cause it starts from 1
            new_mesh.mIndices.push_back(std::stoi(tokens[1])-1);
            new_mesh.mIndices.push_back(std::stoi(tokens[2])-1);
            new_mesh.mIndices.push_back(std::stoi(tokens[3])-1);

            new_mesh.mNumIndices += 3;
        }
    }
    file_stream.close();


        // Calculate Normals if not doesnt exist
    if (!new_mesh.mContainsNormals)
        new_mesh.CalculateVertexNormals();

    model.mMeshes.push_back(std::move(new_mesh));
    


    return 0;
}