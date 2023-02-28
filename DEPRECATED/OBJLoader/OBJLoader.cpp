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

    bool encountered_faces = false;
    
    // We may need to generate a new order of normal/uv indices in order to correctly match the vertex index
    // cause .obj files are dumb
    bool has_uv_indices = false;
    std::vector<DirectX::SimpleMath::Vector2> mReorganizedUVs;
    
    bool has_normal_indices = false;
    std::vector<DirectX::SimpleMath::Vector3> mReorganizedNormals;

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
            new_mesh.mNormals.back().Normalize();
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
            // Vertex Indices only:                               "f v1 v2 v3 ..."
            // Vertex Indices + Texture Indices:                  "f v1/vt1 v2/vt2 v3/vt3 ..."
            // Vertex Indices + Normal Indices:                   "f v1//vn1 v2//vn2 v3//vn3 ..."
            // Vertex Indices + Texture Indices + Normal Indices: "f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ..."
            
            // WHY DOES THIS FILE FORMAT DO THIS???
            // Why can't they just put them in the same order as the vertices x.x
        else if (tokens[0] == "f") {
            const std::regex double_slash_regex("\/\/");
            const std::regex slash_regex("\/");

            // The first time we encounter a face, we need to decide how it's formatted so we can properly read them
            if (!encountered_faces) {
                const std::string double_slash("//");
                const std::string slash("/");

                if (tokens[1].find(double_slash) != std::string::npos) {
                    has_normal_indices = true;  
                }
                else if (tokens[1].find(slash) != std::string::npos) {
                    has_uv_indices = true;

                    // Tokenize the string to find out if it also has normal indices, in addition to uv indices
                    std::sregex_token_iterator face_token_it(tokens[1].begin(), tokens[1].end(), slash_regex, -1);
                    std::vector<std::string> face_tokens(face_token_it, end);

                    if (face_tokens.size() == 3)
                        has_normal_indices = true;
                }

                encountered_faces = true;
            }
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // 4 indices cause there might be quads FOR SOME REASON
                // I thought it was good practice for artists to triangulate their meshes
                // WHYYY ARE THERE QUAAAAAAAAAAAAAAAAAAAAAAAAADS
                // we aren't supporting anything more than quads
            uint32_t indices[4] = { 0 };

                // We need to generate a new order of normal/uv indices in order to correctly match the vertex index
            if (has_normal_indices && has_uv_indices) {
                std::sregex_token_iterator face_token_it(tokens[1].begin(), tokens[1].end(), slash_regex, -1);
                std::vector<std::string> face_tokens(face_token_it, end);


            }


                // If the faces are quads, split it into triangles
            if (tokens.size() == 5) {
                new_mesh.mIndices.push_back(std::stoi(tokens[1]) - 1);
                new_mesh.mIndices.push_back(std::stoi(tokens[2]) - 1);
                new_mesh.mIndices.push_back(std::stoi(tokens[3]) - 1);

                new_mesh.mIndices.push_back(std::stoi(tokens[1]) - 1);
                new_mesh.mIndices.push_back(std::stoi(tokens[3]) - 1);
                new_mesh.mIndices.push_back(std::stoi(tokens[4]) - 1);

                new_mesh.mNumIndices += 3;
            }
            else {
                // Subtract 1 from each index cause it starts from 1
                new_mesh.mIndices.push_back(std::stoi(tokens[1]) - 1);
                new_mesh.mIndices.push_back(std::stoi(tokens[2]) - 1);
                new_mesh.mIndices.push_back(std::stoi(tokens[3]) - 1);

                new_mesh.mNumIndices += 3;
            }
        }
    }
    file_stream.close();


        // Calculate Normals if not doesnt exist
    if (!new_mesh.mContainsNormals)
        new_mesh.CalculateVertexNormals();

    model.mMeshes.push_back(std::move(new_mesh));
    


    return 0;
}