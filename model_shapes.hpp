#pragma once

#include "lve_device.hpp"
#include "lve_model.hpp"

#include <glm/glm.hpp>
#include <array>

namespace lve {
	std::unique_ptr<LveModel> createSierpPyramidModel(LveDevice& device, const std::array<LveModel::Vertex, 4>& peaks, int depth);
	std::unique_ptr<LveModel> createSierpPyramidModel(LveDevice& device, float side, int depth);
}