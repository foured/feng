#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>

#include <iostream>

namespace feng {

	class shader {
	public:
		shader(const char* vertex_shader_path, const char* fragment_shader_path);

		void activate();

		void set_bool(const std::string& name, bool val);
		void set_mat4(const std::string& name, glm::mat4 val);
		void set_int(const std::string& name, int value);
		void set_float(const std::string& name, float value);
		void set_4float(const std::string& name, float v1, float v2, float v3, float v4);
		void set_4float(const std::string& name, glm::vec4 v);
		void set_4float(const std::string& name, aiColor4D c);
		void set_3float(const std::string& name, float v1, float v2, float v3);
		void set_3float(const std::string& name, glm::vec3 v);
		void set_ubo_index(const std::string& name, int v);

	private:
		uint32_t _shader_program;

	private:
		std::string load_shader_from_file(const char* filepath);
		uint32_t compile_shader(const char* filepath, uint32_t shader_type);
	};

}