#pragma once

#include <assimp/scene.h>

#include <string>

#include "mesh.h"

#define MAX_NO_MODEL_INSTANCES 10

namespace feng {

	class model {
	public:
		model(std::string filepath, bool has_textures = true);

		void render(shader& shader);
		void add_instance(glm::vec3 position);

	private:
		std::vector<mesh> _meshes;
		std::string _directory;

		glm::vec3 _color;
		bool _has_textures;

		uint32_t _no_instances = 0;
		std::vector<glm::vec3> _positions;
		arraybuffer _pos_array_buffer;

		std::vector<texture> _textures_loaded;

		void setup();
		void load_model(std::string path);
		void process_node(aiNode* node, const aiScene* scene, glm::mat4 matrix);
		mesh process_mesh(aiMesh* mesh, const aiScene* scene, glm::mat4 matrix);
		std::vector<texture> load_material_textures(aiMaterial* mat, aiTextureType type);
	};

}