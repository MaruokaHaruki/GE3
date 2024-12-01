#pragma once
#include <vector>
#include "MaterialData.h"
#include "VertexData.h"

/// <summary>
/// ModelData
/// </summary>
struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};