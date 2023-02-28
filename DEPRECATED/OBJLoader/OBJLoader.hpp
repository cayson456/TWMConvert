#pragma once
#include "Model/Model.hpp"

namespace OBJLoader
{
    int LoadModel(Model &model, const std::filesystem::path& filepath);
};