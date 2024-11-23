#pragma once

#include <assimp/scene.h>

#include <string>

#include "../fng.h"
#include "mesh.h"
#include "../utilities/uuid.hpp"

namespace feng {

	struct advanced_static_vertex_data {
		vertex vert;
		aiColor4D diffuse;
		aiColor4D specular;
		float textures_indexes;
	};

	struct mesh_batch {
		std::vector<advanced_static_vertex_data> asvds;
		std::vector<uint32_t> indices;
		std::vector<texture> textures;

		vertexarray vertex_array;
		elementarraybuffer element_buffer;
		arraybuffer array_buffer;

		void add_mesh(mesh& mesh);
		void setup_data();
		void render(shader& shader, uint32_t no_instances);
	};

	class model : public util::uuid_owner {
	public:
		model(std::string filepath, 
			glm::vec3 initial_instance_pos = glm::vec3(0),
			glm::vec3 initial_instance_size = glm::vec3(1));

		model(std::vector<mesh> meshes,
			glm::vec3 initial_instance_pos = glm::vec3(0),
			glm::vec3 initial_instance_size = glm::vec3(1));

		void render(shader& shader, bool face_culling = true);
		void render_flag(shader& shader, inst_flag_type flag);
		void add_instance(glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1), glm::vec3 rotation = glm::vec3(0));

	private:
		std::vector<mesh> _meshes;
		std::vector<mesh_batch> _batches;
		std::string _directory;
		uint32_t _no_instances = 0;

		std::vector<glm::vec3> _positions;
		arraybuffer _pos_array_buffer;
		std::vector<glm::vec3> _sizes;
		arraybuffer _size_array_buffer;
		std::vector<glm::vec3> _rotations;
		arraybuffer _rot_array_buffer;

		std::vector<texture> _textures_loaded;

		void allocate_buffers();
		void update_instances_buffers();
		void setup();
		void load_model(std::string path);
		void process_node(aiNode* node, const aiScene* scene, glm::mat4 matrix);
		mesh process_mesh(aiMesh* mesh, const aiScene* scene, glm::mat4 matrix);
		std::vector<texture> load_material_textures(aiMaterial* mat, aiTextureType type);
		void batch_meshes();
	};

}