#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <algorithm>

#include "../logging/logging.h"
#include "../logic/data_management/files.h"

namespace feng {

	model::model() {

	}

	model::model(std::string filepath, bool disable_faceculling) 
		: _disable_faceculling(disable_faceculling) {
		load_model(filepath);
		setup();
		calculate_bounds();
		LOG_ACTION("Loaded model: '" + filepath + "'. " + get_uuid_string());
	}

	model::model(std::vector<mesh> meshes, bool disable_faceculling)
		: _meshes(meshes), _disable_faceculling(disable_faceculling) {
		setup();
		calculate_bounds();
		LOG_ACTION("Loaded model with " + std::to_string(meshes.size()) + " custom meshes. " + get_uuid_string());
	}

	void model::setup(bool do_batching) {
		if(do_batching)
			batch_meshes();
		for (mesh_batch& batch : _batches)
			batch.setup_data();

		allocate_buffers();

		for (mesh_batch& batch : _batches) {
			batch.vertex_array.bind();
			_pos_array_buffer.bind();
			batch.vertex_array.set_attrib_pointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0, 1);
			_size_array_buffer.bind();
			batch.vertex_array.set_attrib_pointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0, 1);
			_rot_array_buffer.bind();
			//batch.vertex_array.set_attrib_pointer(9, 3 * 3, GL_FLOAT, GL_FALSE, sizeof(glm::mat3), 0, 1);
			batch.vertex_array.set_atrib_pointer_matrix<4>(9, 0, 1);
			vertexarray::unbind();
		}
	}

	void model::allocate_buffers() {
		_pos_array_buffer.generate();
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_data<glm::vec3>(MAX_NO_MODEL_INSTANCES * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

		_size_array_buffer.generate();
		_size_array_buffer.bind();
		_size_array_buffer.buffer_data<glm::vec3>(MAX_NO_MODEL_INSTANCES * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

		_rot_array_buffer.generate();
		_rot_array_buffer.bind();
		_rot_array_buffer.buffer_data<glm::mat4>(MAX_NO_MODEL_INSTANCES * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	}

	void model::update_instances_buffers() {
		uint32_t no_instances = _positions.size();
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_sub_data(0, sizeof(glm::vec3) * no_instances, &_positions[0]);

		_size_array_buffer.bind();
		_size_array_buffer.buffer_sub_data(0, sizeof(glm::vec3) * no_instances, &_sizes[0]);

		_rot_array_buffer.bind();
		_rot_array_buffer.buffer_sub_data(0, sizeof(glm::mat4) * no_instances, &_rotations[0]);
	}

	void model::add_instance(instance* i) {
		_instances.push_back(i);
	}

	void model::clear_instances() {
		_instances.clear();
	}

	void model::serialize(data::wfile* file) {
		file->write_raw(get_uuid());
		file->write_raw(_disable_faceculling);
		file->write_raw(_batches.size());
		for (auto& b : _batches) {
			file->write_raw(b.asvds.size());
			for (const auto& asvd : b.asvds)
				file->write_raw(asvd);
			file->write_raw(b.indices.size());
			for (auto i : b.indices)
				file->write_raw(i);
			file->write_raw(b.textures.size());
			for (auto& t : b.textures)
				file->write_serializable(&t);
		}
	}

	void model::deserialize(data::rfile* file, scene* scene) {
		uuid_type uuid;
		file->read_raw(&uuid);
		reset(uuid);
		file->read_raw(&_disable_faceculling);
		size_t batches_size;
		file->read_raw(&batches_size);
		_batches = std::vector<mesh_batch>(batches_size);
		for (size_t bi = 0; bi < batches_size; bi++) {
			size_t size;
			// advanced_static_vertex_data
			file->read_raw(&size);
			_batches[bi].asvds = std::vector<advanced_static_vertex_data>(size);
			file->read_raw((char*)_batches[bi].asvds.data(), size * sizeof(advanced_static_vertex_data));
			// indices
			file->read_raw(&size);
			_batches[bi].indices = std::vector<uint32_t>(size);
			file->read_raw((char*)_batches[bi].indices.data(), size * sizeof(uint32_t));
			// textures
			file->read_raw(&size);
			_batches[bi].textures = std::vector<texture>(size);
			for (size_t ti = 0; ti < size; ti++) {
				file->read_serializable(&_batches[bi].textures[ti], scene);
			}
		}
		setup(false);
		calculate_bounds();
		LOG_ACTION("Model loaded " + get_uuid_string());
	}

	void model::load_model(std::string path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, 
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG_ERROR("ASSIMP error: " + std::string(importer.GetErrorString()));
			return;
		}
		_directory = path.substr(0, path.find_last_of('/'));

		process_node(scene->mRootNode, scene, glm::mat4(1.0f));
	}

	void model::process_node(aiNode* node, const aiScene* scene, glm::mat4 matrix)
	{
		aiMatrix4x4 n_mat_ai = node->mTransformation;
		glm::mat4 n_mat = glm::mat4(
			n_mat_ai.a1, n_mat_ai.a2, n_mat_ai.a3, n_mat_ai.a4,
			n_mat_ai.b1, n_mat_ai.b2, n_mat_ai.b3, n_mat_ai.b4,
			n_mat_ai.c1, n_mat_ai.c2, n_mat_ai.c3, n_mat_ai.c4,
			n_mat_ai.d1, n_mat_ai.d2, n_mat_ai.d3, n_mat_ai.d4
		) * matrix;

		for (size_t i = 0; i < node->mNumMeshes; i++){
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			_meshes.emplace_back(process_mesh(mesh, scene, n_mat));
		}
		for (size_t i = 0; i < node->mNumChildren; i++){
			process_node(node->mChildren[i], scene, n_mat);
		}
	}

	mesh model::process_mesh(aiMesh* mesh, const aiScene* scene, glm::mat4 matrix) {
		std::vector<vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<texture> textures;

		for (size_t i = 0; i < mesh->mNumVertices; i++) {
			vertex vert;
			glm::vec3 vector;

			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vert.position = glm::vec4(vector, 1.0f) * matrix;

			if (mesh->HasNormals()) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vert.normal = vector * glm::mat3(matrix);
			}

			glm::vec2 vec(0.0f);
			if (mesh->mTextureCoords[0]) {
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
			}
			vert.tex_coords = vec;

			if (mesh->HasTangentsAndBitangents()) {
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vert.tangent = vector * glm::mat3(matrix);
			}

			vertices.push_back(vert);
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				std::vector<texture> diffuse_maps
					= load_material_textures(material, aiTextureType_DIFFUSE);
				textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
				std::vector<texture> specular_maps
					= load_material_textures(material, aiTextureType_SPECULAR);
				textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
				std::vector<texture> normal_maps
					= load_material_textures(material, aiTextureType_NORMALS);
				textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
				return feng::mesh(vertices, indices, textures);
			}
			else {
				aiColor4D diff(1.0f);
				aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);
				aiColor4D spec(1.0f);
				aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

				return feng::mesh(vertices, indices, diff, spec);
			}
		}
	}

	std::vector<texture> model::load_material_textures(aiMaterial* mat, aiTextureType type) {
		std::vector<texture> textures;
		for (size_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (size_t j = 0; j < _textures_loaded.size(); j++)
			{
				if (std::strcmp(_textures_loaded[j].path().data(), str.C_Str()) == 0)
				{
					textures.push_back(_textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				texture current_texture(_directory, str.C_Str(), type);
				textures.push_back(current_texture);
				_textures_loaded.push_back(current_texture);
			}
		}
		return textures;
	}

	void model::render_ready_data(shader& shader) {
		uint32_t no_instances = _positions.size();
		if (no_instances > 0) {
			if (_disable_faceculling) glDisable(GL_CULL_FACE);

			update_instances_buffers();

			for (mesh_batch& batch : _batches) {
				batch.render(shader, no_instances);
			}

			if (_disable_faceculling) glEnable(GL_CULL_FACE);
		}
	}

	void model::render(shader& shader) {
		clear_instances_data();
		for (const auto& i : _instances) {
			add_instance_render_data(i);
		}
		render_ready_data(shader);
	}

	void model::render_flag(shader& shader, inst_flag_type flag) {
		clear_instances_data();
		for (const auto& i : _instances) {
			if (i->flags.get(flag)) {
				add_instance_render_data(i);
			}
		}
		render_ready_data(shader);
	}

	void model::render_single_instance(shader& shader, instance* instance) {
		clear_instances_data();
		add_instance_render_data(instance);
		render_ready_data(shader);
	}

	void model::add_instance_render_data(instance* instance) {
		_positions.push_back(instance->transform.get_position());
		_sizes.push_back(instance->transform.get_size());
		_rotations.push_back(instance->transform.get_rotation_matrix3x3());
	}

	void model::batch_meshes() {
		_batches.emplace_back();

		for (mesh& mesh : _meshes) {
			bool batched = false;
			for (mesh_batch& batch : _batches) {
				uint16_t unique_textures = 0;
				for (texture& tex : mesh._textures) {
					if (std::find(batch.textures.begin(), batch.textures.end(), tex) == batch.textures.end()) {
						unique_textures++;;
					}
				}

				if ((batch.textures.size() + unique_textures) <= MAX_TEXTURE_SLOT_M) {
					batched = true;
					batch.add_mesh(mesh);
				}
			}

			if (!batched) {
				_batches.emplace_back();
				_batches.back().add_mesh(mesh);
			}
		}
	}

	void mesh_batch::add_mesh(mesh& mesh) {
		uint8_t d_idx = NULL_TEXTURE_IDX, s_idx = NULL_TEXTURE_IDX, n_idx = NULL_TEXTURE_IDX;
		for (int i = 0; i < textures.size(); i++) {
			for (texture& mtex : mesh._textures) {
				if (mtex == textures[i]) {
					switch (mtex.aiTtype())
					{
					case aiTextureType_DIFFUSE:
						d_idx = i;
						break;
					case aiTextureType_SPECULAR:
						s_idx = i;
						break;
					case aiTextureType_NORMALS:
						n_idx = i;
						break;
					}
				}
			}
		}
		if (d_idx == NULL_TEXTURE_IDX) {
			for (texture& mtex : mesh._textures) {
				if (mtex.aiTtype() == aiTextureType_DIFFUSE) {
					d_idx = textures.size();
					textures.push_back(mtex);
					break;
				}
			}
		}
		if (s_idx == NULL_TEXTURE_IDX) {
			for (texture& mtex : mesh._textures) {
				if (mtex.aiTtype() == aiTextureType_SPECULAR) {
					s_idx = textures.size();
					textures.push_back(mtex);
					break;
				}
			}
		}
		if (n_idx == NULL_TEXTURE_IDX) {
			for (texture& mtex : mesh._textures) {
				if (mtex.aiTtype() == aiTextureType_NORMALS) {
					n_idx = textures.size();
					textures.push_back(mtex);
					break;
				}
			}
		}

		uint32_t v_offset = asvds.size();

		indices.reserve(mesh._indices.size());
		for (uint32_t v : mesh._indices) {
			indices.emplace_back(v + v_offset);
		}

		int32_t textures_idxs = d_idx | s_idx << 8 | n_idx << 16;
		advanced_static_vertex_data nasvd;
		if (!mesh._has_textures) {
			nasvd.diffuse = mesh._diffuse;
			nasvd.specular = mesh._specular;
		}
		nasvd.textures_indexes = textures_idxs;
		asvds.reserve(mesh._vertices.size());
		for (size_t i = 0; i < mesh._vertices.size(); i++) {
			nasvd.vert = mesh._vertices[i];
			asvds.emplace_back(nasvd);
		}
	}

	void mesh_batch::setup_data() {
		vertex_array.generate();
		vertex_array.bind();

		element_buffer.generate();
		element_buffer.bind();
		element_buffer.buffer_data(indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

		uint32_t s = sizeof(advanced_static_vertex_data);

		array_buffer.generate();
		array_buffer.bind();
		array_buffer.buffer_data(asvds.size() * s, &asvds[0], GL_STATIC_DRAW);

		vertex_array.set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, s, 0);
		vertex_array.set_attrib_pointer(1, 3, GL_FLOAT, GL_FALSE, s, (3 * sizeof(float)));
		vertex_array.set_attrib_pointer(2, 2, GL_FLOAT, GL_FALSE, s, (6 * sizeof(float)));
		vertex_array.set_attrib_pointer(3, 3, GL_FLOAT, GL_FALSE, s, (8 * sizeof(float)));
		vertex_array.set_attrib_pointer(4, 4, GL_FLOAT, GL_FALSE, s, (11 * sizeof(float)));
		vertex_array.set_attrib_pointer(5, 4, GL_FLOAT, GL_FALSE, s, (15 * sizeof(float)));
		vertex_array.set_attrib_pointer(6, 1, GL_FLOAT, GL_FALSE, s, (19 * sizeof(float)));

		vertexarray::unbind();
	}

	void mesh_batch::render(shader& shader, uint32_t no_instances) {
		for (size_t i = 0; i < textures.size(); i++) {
			shader.set_int("textures[" + std::to_string(i) + "]", i);
			glActiveTexture(GL_TEXTURE0 + i);
			textures[i].bind();
		}

		vertex_array.bind();
		vertex_array.draw_elements_instanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, no_instances);

		vertexarray::unbind();
		glActiveTexture(GL_TEXTURE0);
	}

	void model::clear_instances_data() {
		_positions.clear();
		_sizes.clear();
		_rotations.clear();
	}

	void model::calculate_bounds() {
		bounds.set_numeric_limits();
		for (const auto& batch : _batches) {
			for (const auto& asvd : batch.asvds) {
				bounds.min = glm::min(bounds.min, asvd.vert.position);
				bounds.max = glm::max(bounds.max, asvd.vert.position);
			}
		}
	}

}