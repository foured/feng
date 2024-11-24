#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "../logging/logging.h"

namespace feng::util {
	namespace fs = std::filesystem;

	class files_in_folder {
	public:
		files_in_folder(const char* path) {
			search(path);
		}

		std::string read_file(const std::string& file_name) {
			if (!_files.contains(file_name))
				THROW_ERROR("Error to find file: " + file_name);
			const auto& path = _files[file_name];
			std::ifstream file(path);
			if (!file.is_open())
				THROW_ERROR("Error to open file: " + file_name);
			std::stringstream buffer;
			buffer << file.rdbuf();
			return buffer.str();
		}

	public:
		std::unordered_map<std::string, fs::path> _files;

		void search(fs::path path) {
			for (const auto& entry : fs::directory_iterator(path)) {
				if (entry.is_directory()) {
					search(entry.path());
				}
				else {
					_files.insert({ entry.path().filename().string() , entry.path() });
				}
			}
		}

	};

}