#pragma once

#include "bve_device.hpp"
#include "bve_model.hpp"

#include <glm/glm.hpp>
#include <array>

namespace bve {
	std::unique_ptr<BveModel> createSierpPyramidModel(BveDevice& device, const std::array<BveModel::Vertex, 4>& peaks, int depth);
	std::unique_ptr<BveModel> createSierpPyramidModel(BveDevice& device, float side, int depth);
}