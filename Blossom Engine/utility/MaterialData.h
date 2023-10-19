#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include "../Blossom Engine/Math/MatrixCalculate.h"

struct MaterialData {
	std::string textureFilePath;
};

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);