#pragma once

#include "../../utilities/singleton.hpp"
#include "../../utilities/files_in_folder.hpp"

namespace feng {

	class assets_manager : public util::singleton<assets_manager> {
	public:
		assets_manager() 
			: _shader_folder("res/shaders") { }

		std::string get_shader(const std::string file_name) {
			return _shader_folder.read_file(file_name);
		}

	private:
		util::files_in_folder _shader_folder;
	};

}