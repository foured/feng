#include "shader.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>
#include <string>

#include "../logging/logging.h"

namespace feng {

	shader::shader(const char* vertex_shader_path, const char* fragment_shader_path) {
		uint32_t vertex_shader = compile_shader(vertex_shader_path, GL_VERTEX_SHADER);
		uint32_t fragment_shader = compile_shader(fragment_shader_path, GL_FRAGMENT_SHADER);

		_shader_program = glCreateProgram();

		glAttachShader(_shader_program, vertex_shader);
		glAttachShader(_shader_program, fragment_shader);
		glLinkProgram(_shader_program);

		int32_t success;
		char info_log[512];
		glGetProgramiv(_shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(_shader_program, 512, NULL, info_log);
			LOG_ERROR("Error to attach shaders:\n " + std::string(info_log));
		}

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);

		LOG_ACTION("Compiled shader: '" + std::string(vertex_shader_path) + "', '"
			+ std::string(vertex_shader_path) + "'.");
	}

	std::string shader::load_shader_from_file(const char* filepath) {
		std::ifstream file;
		std::stringstream buf;

		std::string ret = "";

		file.open(filepath);

		if (file.is_open()) {
			buf << file.rdbuf();
			ret = buf.str();
		}
		else {
			LOG_ERROR("Couldn`t open " + std::string(filepath));
		}

		file.close();

		return ret;
	}

	uint32_t shader::compile_shader(const char* filepath, uint32_t shader_type) {
		std::string shader_src_s = load_shader_from_file(filepath);
		const char* shader_src = shader_src_s.c_str();

		uint32_t shader;
		shader = glCreateShader(shader_type);

		glShaderSource(shader, 1, &shader_src, NULL);
		glCompileShader(shader);

		int32_t success;
		char info_log[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderInfoLog(shader, 512, NULL, info_log);
			LOG_ERROR("Error to compile shader '" + std::string(filepath) + "': " + std::string(info_log));
		}

		return shader;
	}

	void shader::activate() {
		glUseProgram(_shader_program);
	}

	//
	// don`t look down
	//

	void shader::set_bool(const std::string& name, bool val){
		glUniform1i(glGetUniformLocation(_shader_program, name.c_str()), (int)val);
	}

	void shader::set_mat4(const std::string& name, glm::mat4 val){
		glUniformMatrix4fv(glGetUniformLocation(_shader_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
	}

	void shader::set_int(const std::string& name, int value){
		glUniform1i(glGetUniformLocation(_shader_program, name.c_str()), value);
	}

	void shader::set_float(const std::string& name, float value){
		glUniform1f(glGetUniformLocation(_shader_program, name.c_str()), value);
	}

	void shader::set_4float(const std::string& name, float v1, float v2, float v3, float v4){
		glUniform4f(glGetUniformLocation(_shader_program, name.c_str()), v1, v2, v3, v4);
	}

	void shader::set_4float(const std::string& name, glm::vec4 v){
		glUniform4f(glGetUniformLocation(_shader_program, name.c_str()), v.x, v.y, v.z, v.w);
	}

	void shader::set_4float(const std::string& name, aiColor4D c) {
		glUniform4f(glGetUniformLocation(_shader_program, name.c_str()), c.r, c.g, c.b, c.a);
	}

	void shader::set_3float(const std::string& name, float v1, float v2, float v3){
		glUniform3f(glGetUniformLocation(_shader_program, name.c_str()), v1, v2, v3);
	}

	void shader::set_3float(const std::string& name, glm::vec3 v){
		glUniform3f(glGetUniformLocation(_shader_program, name.c_str()), v.x, v.y, v.z);
	}

	void shader::set_ubo_index(const std::string& name, int v) {
		glUniformBlockBinding(_shader_program, glGetUniformBlockIndex(_shader_program, name.c_str()), v);
	}

	uint32_t shader::id() const {
		return _shader_program;
	}

}