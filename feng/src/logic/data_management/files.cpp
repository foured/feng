#include "files.h"

namespace feng::data {

	// Write file

	wfile::wfile(const std::filesystem::path& filepath)
	{
		_stream = std::ofstream(filepath, std::iostream::out | std::iostream::binary);
	}

	wfile::~wfile()
	{
		_stream.close();
	}

	void wfile::write_raw(const char* data, size_t size)
	{
		_stream.write(data, size);
	}

	void wfile::write_string(const std::string& data)
	{
		size_t size = data.size();
		_stream.write((char*) &size, sizeof(size_t));
		_stream.write(data.data(), sizeof(char) * data.size());
	}

	void wfile::write_serializable(serializable* serializable)
	{
		serializable->serialize(this);
	}

	// Read file
	
	rfile::rfile(const std::filesystem::path& filepath)
	{
		_stream = std::ifstream(filepath, std::iostream::in | std::iostream::binary);
	}

	rfile::~rfile()
	{
		_stream.close();
	}

	void rfile::read_raw(char* destination, size_t size)
	{
		_stream.read(destination, size);
	}

	void rfile::read_string(std::string& string)
	{
		size_t size;
		_stream.read((char*)&size, sizeof(size_t));
		string.resize(size);
		_stream.read(string.data(), sizeof(char) *  size);
	}

	void rfile::read_serializable(serializable* serializable)
	{
		serializable->deserialize(this);
	}

}