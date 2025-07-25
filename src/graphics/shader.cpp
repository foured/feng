#include "shader.h"

#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include "../logging/logging.h"
#include "../utilities/utilities.h"
#include "../logic/data_management/assets_manager.h"
#include "../logic/data_management/files.h"
#include "gl_buffers/shader_storage.hpp"

#define SHADER_INCLUDE_WORD "#include"
#define SHADER_INCLUDE_OPEN_BRACKET_CHAR '<'
#define SHADER_INCLUDE_CLOSE_BRACKET_CHAR '>'

#define SHADER_EXTERNA_WORD "#external define"
#define SHADER_EXTERNAL_OPEN_BRACKET_CHAR '['
#define SHADER_EXTERNAL_CLOSE_BRACKET_CHAR ']'

namespace feng {

	shader_sub_program::shader_sub_program(const char* path, uint32_t type) 
		: path(path), type(type) {	}

	std::unordered_map<std::string, std::string> shader::_external_defines = {};

	shader::shader() {}

	shader::shader(const char* vertex_shader_path, const char* fragment_shader_path, std::vector<std::string> defines) {
		load_sub_programs({ shader_sub_program(vertex_shader_path, GL_VERTEX_SHADER), shader_sub_program(fragment_shader_path, GL_FRAGMENT_SHADER)}, defines);
	}

	shader::shader(const char* vertex_shader_path, const char* fragment_shader_path, 
		std::vector<shader_sub_program> additional_progs, std::vector<std::string> defines) {
		additional_progs.push_back(shader_sub_program(vertex_shader_path, GL_VERTEX_SHADER));
		additional_progs.push_back(shader_sub_program(fragment_shader_path, GL_FRAGMENT_SHADER));
		load_sub_programs(additional_progs, defines);
	}

	void shader::load_sub_programs(const std::vector<shader_sub_program>& additional_progs, const std::vector<std::string>& defines) {
		std::vector<uint32_t> shader_progs;
		for (const auto& sp : additional_progs) {
			shader_progs.push_back(compile_shader(sp.path, sp.type, defines));
		}

		_shader_program = glCreateProgram();
		for (auto sp : shader_progs) {
			glAttachShader(_shader_program, sp);
		}
		glLinkProgram(_shader_program);

		int32_t success;
		char info_log[512];
		glGetProgramiv(_shader_program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(_shader_program, 512, NULL, info_log);
			LOG_ERROR("Error to attach shaders:\n " + std::string(info_log));
		}

		for (auto sp : shader_progs) {
			glDeleteShader(sp);
		}

		std::string res_str = "Compiled shader: ";
		for (const auto& sp : additional_progs)
			res_str += "'" + std::string(sp.path) + "' ";
		LOG_ACTION(res_str);
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
			LOG_ERROR("Couldn`t open file: ", strerror(errno), " ", filepath);
		}

		file.close();

		include_headers_and_external_defines(ret);
		return ret;
	}

	std::string shader::include_headers_and_external_defines(std::string& shader_code) {
		size_t include_pos = shader_code.find(SHADER_INCLUDE_WORD);
		std::vector<std::string> included_symbols;
		while (include_pos != std::string::npos) {
			size_t ob_pos = shader_code.find(SHADER_INCLUDE_OPEN_BRACKET_CHAR, include_pos);
			size_t cb_pos = shader_code.find(SHADER_INCLUDE_CLOSE_BRACKET_CHAR, include_pos);
			if ((ob_pos == std::string::npos) || (cb_pos == std::string::npos)) {
				LOG_ERROR("Error to find include brackets in shader.");
				return shader_code;
			}
			std::string include_name = utilities::strip(shader_code.substr(ob_pos + 1, cb_pos - ob_pos - 1));
			if (std::find(included_symbols.begin(), included_symbols.end(), include_name) == included_symbols.end()) {
				shader_code.replace(
					include_pos, cb_pos - include_pos + 1, '\n' + assets_manager::get_instance()->get_shader(include_name) + '\n');
				included_symbols.emplace_back(include_name);
			}
			else {
				shader_code.replace(include_pos, cb_pos - include_pos + 1, "");
			}
			include_pos = shader_code.find(SHADER_INCLUDE_WORD);
		}

		included_symbols.clear();
		include_pos = shader_code.find(SHADER_EXTERNA_WORD);
		while(include_pos != std::string::npos){
			size_t ob_pos = shader_code.find(SHADER_EXTERNAL_OPEN_BRACKET_CHAR, include_pos);
			size_t cb_pos = shader_code.find(SHADER_EXTERNAL_CLOSE_BRACKET_CHAR, include_pos);
			if ((ob_pos == std::string::npos) || (cb_pos == std::string::npos)) {
				LOG_ERROR("Error to find include brackets in shader.");
				return shader_code;
			}
			std::string include_name = utilities::strip(shader_code.substr(ob_pos + 1, cb_pos - ob_pos - 1));
			if(std::find(included_symbols.begin(), included_symbols.end(), include_name) == included_symbols.end()){
				std::string define = "#define " + include_name + " " + get_external_define(include_name);
				shader_code.replace(include_pos, cb_pos - include_pos + 1, define);
				included_symbols.push_back(include_name);
			} else{
				shader_code.replace(include_pos, cb_pos - include_pos + 1, "");
			}
			include_pos = shader_code.find(SHADER_EXTERNA_WORD);
		}

		return "";
	}

	uint32_t shader::compile_shader(const char* filepath, uint32_t shader_type, const std::vector<std::string>& defines) {
		std::string shader_src_s = load_shader_from_file(filepath);
		set_defines(&shader_src_s, defines);
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
			std::string log_filename = "log/log_" + std::filesystem::path(filepath).filename().string();
			data::wfile file(log_filename);

			if (!file) {
				LOG_ERROR("Error to create shader log file!");
			}
			else {
				file.write_raw(shader_src_s.data(), sizeof(char) * shader_src_s.size());
				//output_file << shader_src_s;
				//output_file.close();
				LOG_INFO("Saved shader log file: " + log_filename);
			}

		}

		return shader;
	}

	void shader::set_defines(std::string* target, const std::vector<std::string>& defines) {
		if (!defines.empty()) {
			int32_t leidx = target->find('\n');
			target->insert(leidx, "\n");
			uint32_t offset = 0;
			for (const auto& d : defines) {
				target->insert(leidx + 2 + offset, "#define " + d + "\n");
				offset += d.size() + 9;
			}
		}
	}

	void shader::activate() {
		glUseProgram(_shader_program);
	}

	void shader::add_external_define(const std::string& key, const std::string& val){
		FENG_ASSERT(!_external_defines.contains(key), "Shaders external defines key '", key,"' already exists.");
		_external_defines[key] = val;
	}

	std::string shader::get_external_define(const std::string& key){
		FENG_ASSERT(_external_defines.contains(key), "No shaders external defines key '", key,"' found.");
		return _external_defines.at(key);
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

	void shader::add_ubo(const std::string& name) {
		uint32_t slot = shader_storage::get_name_binding(name);
		glUniformBlockBinding(_shader_program, glGetUniformBlockIndex(_shader_program, name.c_str()), slot);
	}

	uint32_t shader::id() const {
		return _shader_program;
	}

}