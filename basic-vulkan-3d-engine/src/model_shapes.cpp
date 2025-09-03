#include "model_shapes.hpp"


namespace lve {

	std::unique_ptr<LveModel> createSierpPyramidModel(LveDevice& device, const std::array<LveModel::Vertex, 4>& peaks, int depth) {
		//calculate normal for each face - clockwise
		std::array<glm::vec3, 4> faceNormals;
		faceNormals[0] = glm::normalize(glm::cross(peaks[1].position - peaks[0].position, peaks[2].position - peaks[0].position));
		faceNormals[1] = glm::normalize(glm::cross(peaks[2].position - peaks[0].position, peaks[3].position - peaks[0].position));
		faceNormals[2] = glm::normalize(glm::cross(peaks[3].position - peaks[0].position, peaks[1].position - peaks[0].position));
		faceNormals[3] = glm::normalize(glm::cross(peaks[3].position - peaks[1].position, peaks[2].position - peaks[1].position));



		int nPyramids = static_cast<uint32_t>(std::pow(4, depth));
		std::vector<std::array<LveModel::Vertex, 4>> outPyramidVector;
		outPyramidVector.reserve(nPyramids);
		outPyramidVector.emplace_back(peaks);

		std::vector<std::array<LveModel::Vertex, 4>> helpPyramidVector;
		helpPyramidVector.reserve(nPyramids / 3);

		// subdivide into 4 smaller pyramids depth times
		for (size_t i = 0; i < depth; i++)
		{
			// divide each pyramid in outPyramidVector into 4 smaller
			for (const std::array<LveModel::Vertex, 4>&pyramidVertexes : outPyramidVector) {
				// pyramid 1
				helpPyramidVector.push_back({
					pyramidVertexes[0],
					(pyramidVertexes[0] + pyramidVertexes[1]) / 2.f,
					(pyramidVertexes[0] + pyramidVertexes[2]) / 2.f,
					(pyramidVertexes[0] + pyramidVertexes[3]) / 2.f
					});
				// pyramid 2
				helpPyramidVector.push_back({
					(pyramidVertexes[1] + pyramidVertexes[0]) / 2.f,
					pyramidVertexes[1],
					(pyramidVertexes[1] + pyramidVertexes[2]) / 2.f,
					(pyramidVertexes[1] + pyramidVertexes[3]) / 2.f
					});
				// pyramid 3
				helpPyramidVector.push_back({
					(pyramidVertexes[2] + pyramidVertexes[0]) / 2.f,
					(pyramidVertexes[2] + pyramidVertexes[1]) / 2.f,
					pyramidVertexes[2],
					(pyramidVertexes[2] + pyramidVertexes[3]) / 2.f
					});
				// pyramid 4
				helpPyramidVector.push_back({
					(pyramidVertexes[3] + pyramidVertexes[0]) / 2.f,
					(pyramidVertexes[3] + pyramidVertexes[1]) / 2.f,
					(pyramidVertexes[3] + pyramidVertexes[2]) / 2.f,
					pyramidVertexes[3]
					});
			}
			outPyramidVector = helpPyramidVector;
			helpPyramidVector.clear();
		}

		LveModel::Builder modelBuilder{};
		modelBuilder.vertices.reserve(nPyramids * 4);
		modelBuilder.indices.reserve(nPyramids * 4 * 3);

		int pyramidIdx = 0;
		for (const std::array<LveModel::Vertex, 4>&pyramidVertexes : outPyramidVector) {

			// create triangles indices
			// triangle face 1
			modelBuilder.vertices.emplace_back(pyramidVertexes[0]);
			modelBuilder.vertices.back().normal = faceNormals[0];
			modelBuilder.vertices.emplace_back(pyramidVertexes[1]);
			modelBuilder.vertices.back().normal = faceNormals[0];
			modelBuilder.vertices.emplace_back(pyramidVertexes[2]);
			modelBuilder.vertices.back().normal = faceNormals[0];

			// triangle face 2
			modelBuilder.vertices.emplace_back(pyramidVertexes[0]);
			modelBuilder.vertices.back().normal = faceNormals[1];
			modelBuilder.vertices.emplace_back(pyramidVertexes[2]);
			modelBuilder.vertices.back().normal = faceNormals[1];
			modelBuilder.vertices.emplace_back(pyramidVertexes[3]);
			modelBuilder.vertices.back().normal = faceNormals[1];

			// triangle face 3
			modelBuilder.vertices.emplace_back(pyramidVertexes[0]);
			modelBuilder.vertices.back().normal = faceNormals[2];
			modelBuilder.vertices.emplace_back(pyramidVertexes[3]);
			modelBuilder.vertices.back().normal = faceNormals[2];
			modelBuilder.vertices.emplace_back(pyramidVertexes[1]);
			modelBuilder.vertices.back().normal = faceNormals[2];

			// triangle face 4
			modelBuilder.vertices.emplace_back(pyramidVertexes[3]);
			modelBuilder.vertices.back().normal = faceNormals[3];
			modelBuilder.vertices.emplace_back(pyramidVertexes[2]);
			modelBuilder.vertices.back().normal = faceNormals[3];
			modelBuilder.vertices.emplace_back(pyramidVertexes[1]);
			modelBuilder.vertices.back().normal = faceNormals[3];
		}

		return std::make_unique<LveModel>(device, modelBuilder);
	}
	std::unique_ptr<LveModel> createSierpPyramidModel(LveDevice& device, float side, int depth)
	{

		float height = side * 0.8165f;
		float sideHeight = side * .8660f;
		std::array<LveModel::Vertex, 4> peaks;
		peaks[0] = { {.0f,-height * 0.5f,.0f},{1.f,1.f,1.f} };
		peaks[1] = { {.0f,height * 0.5f, sideHeight * 0.666f},{1.f,0.f,0.f} };
		peaks[2] = { {-side * .5f,height * 0.5f,-sideHeight * 0.333f},{0.f,1.f,0.f} };
		peaks[3] = { {side * .5f,height * 0.5f,-sideHeight * 0.333f},{0.f,0.f,1.f} };
		return createSierpPyramidModel(device, peaks, depth);
	}
}