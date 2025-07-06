#pragma once

#include <string>

//https://github.com/StudioCherno/Walnut/blob/dev/Walnut/Source/Walnut/Core/Buffer.h

namespace feng::data {

	struct buffer {
		void* data;
		uint64_t size;

		buffer() 
			: data(nullptr), size(0) { }

		buffer(void* data, uint64_t size) 
			: data(data), size(size) { }

		void allocate(uint64_t size) {
			delete[](char*)data;
			data = nullptr;
			if (size == 0)
				return;
			data = new uint8_t[size];
			this->size = size;
		}

		void write(const void* Data, uint64_t size, uint64_t offset = 0) {
			memcpy((uint8_t*)data + offset, Data, size);
		}

		void free() {
			delete[](char*)data;
			data = nullptr;
			size = 0;
		}

		static buffer copy(const buffer& other) {
			buffer buffer;
			buffer.allocate(other.size);
			memcpy(buffer.data, other.data, other.size);
			return buffer;
		}

		static buffer copy(const void* Data, uint64_t Size) {
			buffer buffer;
			buffer.allocate(Size);
			memcpy(buffer.data, Data, Size);
			return buffer;
		}

		operator bool() const { return data; }

		template<typename T>
		T* as() {
			return (T*)data;
		}

	};

}