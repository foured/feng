#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "../logging/logging.h"

namespace feng {

	model::model(std::string filepath, bool has_textures)
		: _has_textures(has_textures) {
		START_TIMER();
		load_model(filepath);
		add_instance(glm::vec3(0.0f));
		setup();
		LOG_INFO("Model at '" + filepath + "' has " + std::to_string(_meshes.size()) + " meshes.");
	}

	void model::setup() {
		_pos_array_buffer.generate();
		_pos_array_buffer.bind();
		// переделать с использованием nullptr
		_pos_array_buffer.buffer_data(MAX_NO_MODEL_INSTANCES * sizeof(glm::vec3), &_positions[0], GL_DYNAMIC_DRAW);

		for (mesh& m : _meshes) {
			m.vertex_array.bind();
			_pos_array_buffer.bind();
			m.vertex_array.set_attrib_pointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0, 1);
			vertexarray::unbind();
		}
	}

	void model::add_instance(glm::vec3 position) {
		_no_instances++;
		_positions.push_back(position);
	}

	void model::load_model(std::string path) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
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
			_meshes.push_back(process_mesh(mesh, scene, n_mat));
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

	void model::render(shader& shader) {
		_pos_array_buffer.bind();
		_pos_array_buffer.buffer_sub_data(0, sizeof(glm::vec3) * _no_instances, &_positions[0]);

		for (mesh& m : _meshes) {
			m.render(shader, _no_instances);
		}
	}

}