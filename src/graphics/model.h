#pragma once

#include <assimp/scene.h>

#include <string>

#include "../fng.h"
#include "mesh.h"
#include "../utilities/uuid.hpp"
#include "../logic/world/instance.h"
#include "../algorithms/aabb.h"
#include "../logic/data_management/serializable.h"

namespace feng {

	class scene;

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

	class model : public util::uuid_owner, public data::serializable {
	public:
		model();
		model(std::string filepath, bool disable_faceculling = false);
		model(std::vector<mesh> meshes, bool disable_faceculling = false);

		aabb bounds;

		void render_ready_data(shader& shader);
		void render(shader& shader);
		void render_flag(shader& shader, inst_flag_type flag);
		void render_single_instance(shader& shader, instance* instance);

		void add_instance(instance* i);
		void clear_instances();

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		std::vector<mesh> _meshes;
		std::vector<mesh_batch> _batches;
		std::string _directory;
		bool _disable_faceculling;

		std::vector<glm::vec3> _positions, _sizes;
		std::vector<glm::mat4> _rotations;
		arraybuffer _pos_array_buffer, _size_array_buffer, _rot_array_buffer;
		std::vector<instance*> _instances;

		std::vector<texture> _textures_loaded;

		void allocate_buffers();
		void update_instances_buffers();
		void setup(bool do_batching = true);
		void load_model(std::string path);
		void process_node(aiNode* node, const aiScene* scene, glm::mat4 matrix);
		mesh process_mesh(aiMesh* mesh, const aiScene* scene, glm::mat4 matrix);
		std::vector<texture> load_material_textures(aiMaterial* mat, aiTextureType type);
		void batch_meshes();
		void clear_instances_data();
		void add_instance_render_data(instance* instance);

		void calculate_bounds();
	};

}