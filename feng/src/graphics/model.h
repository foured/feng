#pragma once

#include <assimp/scene.h>

#include <string>

#include "mesh.h"

#define MAX_NO_MODEL_INSTANCES 10
#define MAX_NO_TEXTURE_UNITS_M 32
#define NULL_TEXTURE_IDX 255

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

	enum model_render_type {
		batched,
		mesh_by_mesh
	};

	class model {
	public:
		model(std::string filepath, 
			model_render_type render_type = model_render_type::batched, 
			glm::vec3 initial_instance_pos = glm::vec3(0),
			glm::vec3 initial_instance_size = glm::vec3(1));

		model(std::vector<mesh> meshes,
			model_render_type render_type = model_render_type::batched,
			glm::vec3 initial_instance_pos = glm::vec3(0),
			glm::vec3 initial_instance_size = glm::vec3(1));

		void render(shader& shader);
		void add_instance(glm::vec3 position = glm::vec3(0), glm::vec3 size = glm::vec3(1));

	private:
		std::vector<mesh> _meshes;
		std::vector<mesh_batch> _batches;
		std::string _directory;
		model_render_type _render_type;
		uint32_t _no_instances = 0;

		std::vector<glm::vec3> _positions;
		arraybuffer _pos_array_buffer;
		std::vector<glm::vec3> _sizes;
		arraybuffer _size_array_buffer;

		std::vector<texture> _textures_loaded;

		void allocate_buffers();
		void update_instances_buffers();
		void render_batched(shader& shader);
		void render_mesh_by_mesh(shader& shader);
		void setup();
		void setup_batched();
		void setup_mesh_by_mesh();
		void load_model(std::string path);
		void process_node(aiNode* node, const aiScene* scene, glm::mat4 matrix);
		mesh process_mesh(aiMesh* mesh, const aiScene* scene, glm::mat4 matrix);
		std::vector<texture> load_material_textures(aiMaterial* mat, aiTextureType type);
		void batch_meshes();
	};

}