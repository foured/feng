#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/scene.h>

#include <unordered_map>
#include <vector>
#include <string>

namespace feng {

	struct shader_sub_program {
		shader_sub_program(const char* path, uint32_t type);

		uint32_t type;
		const char* path;
	};

	class shader {
	public:
		shader();
		shader(const char* vertex_shader_path, const char* fragment_shader_path, std::vector<std::string> defines = std::vector<std::string>());
		shader(const char* vertex_shader_path, const char* fragment_shader_path, 
			std::vector<shader_sub_program> additional_progs, 
			std::vector<std::string> defines);

		void activate();

		static void add_external_define(const std::string& key, const std::string& val);
		static std::string get_external_define(const std::string& key);


		void set_bool(const std::string& name, bool val);
		void set_mat4(const std::string& name, glm::mat4 val);
		void set_int(const std::string& name, int value);
		void set_float(const std::string& name, float value);
		void set_4float(const std::string& name, float v1, float v2, float v3, float v4);
		void set_4float(const std::string& name, glm::vec4 v);
		void set_4float(const std::string& name, aiColor4D c);
		void set_3float(const std::string& name, float v1, float v2, float v3);
		void set_3float(const std::string& name, glm::vec3 v);
		void add_ubo(const std::string& name);

		uint32_t id() const;

	private:
		uint32_t _shader_program;

		static std::unordered_map<std::string, std::string> _external_defines;

		void load_sub_programs(const std::vector<shader_sub_program>& additional_progs, const std::vector<std::string>& defines);
		std::string load_shader_from_file(const char* filepath);
		uint32_t compile_shader(const char* filepath, uint32_t shader_type, const std::vector<std::string>& defines);
		std::string include_headers_and_external_defines(std::string& shader_code);
		void set_defines(std::string* target, const std::vector<std::string>& defines);

	};

}