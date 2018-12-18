#define GLM_ENABLE_EXPERIMENTAL
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <glm/gtx/hash.hpp>

#include <unordered_map>

#include "Model.h"
#include "simple_logger.h"

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

Model* Model_Manager::LoadModel(const char* modelName)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "models/chalet.obj")) {
		slog("%s", warn + err);
	}

	testModel = new Model();

	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (tinyobj::shape_t &shape : shapes)
	{
		for (tinyobj::index_t &index : shape.mesh.indices)
		{
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(testModel->vertices.size());
				testModel->vertices.push_back(vertex);
			}

			testModel->indices.push_back(uniqueVertices[vertex]);
		}
	}

	slog("Loaded model");
	slog("with %i vertices : ", testModel->vertices.size());
	slog("with %i indices: ", testModel->indices.size());

	return testModel;
}

Model* Model_Manager::NewModel()
{
	return {};
}