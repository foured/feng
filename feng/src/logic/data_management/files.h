#pragma once

#include <filesystem>
#include <fstream>
#include <string>

#include "serializable.h"

// Thx to The Cherno
//https://github.com/StudioCherno/Walnut/blob/dev/Walnut/Source/Walnut/Serialization/FileStream.cpp

namespace feng {
	class scene;
}

namespace feng::data {

	class wfile {
	public:
		wfile(const std::filesystem::path& filepath);
		wfile(const wfile&) = delete;
		~wfile();

		template<typename T>
		void write_raw(const T& data) {
			write_raw((char*)&data, sizeof(T));
		}

		void write_raw(const char* data, size_t size);
		void write_string(const std::string& data);
		void write_serializable(serializable* serializable);

		bool is_good() const { return _stream.good(); };
		uint64_t get_position() { return _stream.tellp(); };
		void set_position(uint64_t position) { _stream.seekp(position); };
		void check_stream();

		operator bool() const { return is_good(); }

	private:
		std::ofstream _stream;

	};

	class rfile {
	public:
		rfile(const std::filesystem::path& filepath);
		rfile(const rfile&) = delete;
		~rfile();


		template<typename T>
		void read_raw(T* type) {
			read_raw((char*)type, sizeof(T));
		}

		void read_raw(char* destination, size_t size);
		void read_string(std::string& string);
		void read_serializable(serializable* serializable, scene* scene);

		bool is_good() const { return _stream.good(); };
		uint64_t get_position() { return _stream.tellg(); };
		void set_position(uint64_t position) { _stream.seekg(position); };
		void check_stream();

		operator bool() const { return is_good(); }

	private:
		std::ifstream _stream;

	};
}