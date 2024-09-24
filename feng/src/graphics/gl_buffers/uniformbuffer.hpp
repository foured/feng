#pragma once

#include <map>
#include <typeindex>

#include "gl_buffer.hpp"
#include "../light/lights.h"

#define TID typeid
#define TS sizeof

#define __IS_T_NOT_UBO_STRUCT(T) !std::is_same_v<uniformbuffer_struct, T> && !std::is_base_of<uniformbuffer_struct, T>

#define __ENABLE_IF_T_NOT_UBO_STRUCT_RET_VOID(T)\
	typename std::enable_if<\
		__IS_T_NOT_UBO_STRUCT(T)\
	::value, void>::type

//#define PRINT_BUFFERING_INFO(T) 

namespace feng {

	struct uniformbuffer_element {
		uint32_t size = 0;
		uint32_t aligned_size = 0;
	};

	class uniformbuffer_struct {
	public:
		template <typename... Ts>
		void add_elements() {
			(add_element<Ts>(), ...);
		}

		template<typename T>
		__ENABLE_IF_T_NOT_UBO_STRUCT_RET_VOID(T) add_element() {
			const auto& t = TID(T);
			if (_base_aligments.contains(t)) {
				_children.emplace_back(TS(T), _base_aligments[t]);
				_size += _base_aligments[t];
				_max_child_size = std::max(_max_child_size, _base_aligments[t]);
			}
			else {
				THROW_ERROR("Error to find 'base aligment' for type: " + std::string(t.name()));
			}
		}
		
		template<typename T, int Len>
		__ENABLE_IF_T_NOT_UBO_STRUCT_RET_VOID(T) add_array() {
			auto t = TID(T);
			if (_base_aligments.contains(t)) {
				uint32_t ats = utilities::round_to(_base_aligments[t], 16);
				_children.reserve(Len);
				for(int32_t i = 0; i < Len; i++)
					_children.emplace_back(TS(T), ats );
				_size += ats * Len;
				_max_child_size = std::max(_max_child_size, ats);
			}
			else {
				THROW_ERROR("Error to find 'base aligment' for type: " + t);
			}
		}

		uint32_t size() const {
			return utilities::round_to(_size, 16);
		}

	private:
		std::vector<uniformbuffer_element> _children;
		uint32_t _size = 0;
		uint32_t _max_child_size = 0;

		friend struct uniformbuffer_pack;
		friend struct uniformbuffer;
		static std::map<std::type_index, uint32_t> _base_aligments;

	};

	std::map<std::type_index, uint32_t> uniformbuffer_struct::_base_aligments{
		{ TID(int), 4 },
		{ TID(float), 4 },
		{ TID(glm::vec2), 8 },
		{ TID(glm::vec3), 16 },
		{ TID(glm::vec4), 16 },
		{ TID(glm::mat4), 64 }
	};

	struct uniformbuffer_pack {
	public:
		template<typename T>
		__ENABLE_IF_T_NOT_UBO_STRUCT_RET_VOID(T) add_element() {
			const auto& t = TID(T);
			if (uniformbuffer_struct::_base_aligments.contains(t)) {
				_size += uniformbuffer_struct::_base_aligments[t];
			}
			else {
				THROW_ERROR("Error to find 'base aligment' for type: " + std::string(t.name()));
			}
		}

		void add_element(uniformbuffer_struct& strct) {
			_size += strct.size();
		}

		template<typename T, int Len>
		__ENABLE_IF_T_NOT_UBO_STRUCT_RET_VOID(T) add_array() {
			auto t = TID(T);
			if (uniformbuffer_struct::_base_aligments.contains(t)) {
				uint32_t ats = utilities::round_to(uniformbuffer_struct::_base_aligments[t], 16);
				_size += ats * Len;
			}
			else {
				THROW_ERROR("Error to find 'base aligment' for type: " + t.name());
			}
		}

		template<int Len>
		void add_array(uniformbuffer_struct& strct) {
			_size += strct.size() * Len;
		}

		uint32_t total_size() const {
			return utilities::round_to(_size, 16);
		}

	private:
		uint32_t _size = 0;

	};


	class uniformbuffer : public gl_buffer {
	public:
		uniformbuffer() 
			: gl_buffer(GL_UNIFORM_BUFFER, false) { }

		void fast_setup(uint32_t index, const uniformbuffer_pack& buffer_pack, int32_t draw_mode = GL_STATIC_DRAW, uint32_t offset = 0) {
			generate();
			bind();
			buffer_data<int32_t>(buffer_pack.total_size(), NULL, draw_mode);
			bind_buffer_range(index, offset, buffer_pack.total_size());
			unbind();
		}

		void start_block() {
			bind();
			_offset = 0;
		}

		void end_block() {
			unbind();
			if (!_did_output) {
				std::cout << "Buffer total offset: " << _offset << '\n';
				std::cout << std::endl;
				_did_output = true;
			}
		}

		template<typename T>
		__ENABLE_IF_T_NOT_UBO_STRUCT_RET_VOID(T) buffer_block_element(T* data) {
			const auto& t = TID(T);
			if (uniformbuffer_struct::_base_aligments.contains(t)) {
				buffer_sub_data(_offset, sizeof(T), data);
				_offset += uniformbuffer_struct::_base_aligments[t];

				if(!_did_output)
					std::cout << "Buffered element size of: " << sizeof(T) << " aligment: "
						<< uniformbuffer_struct::_base_aligments[TID(T)] << 
					" offset: " << _offset << '\n';
			}
			else {
				THROW_ERROR("Error to find 'base aligment' for type: " + std::string(t.name()));
			}
		}

		template<typename T>
		void buffer_block_struct(uniformbuffer_struct& s, T* data) {
			uint32_t struct_offset = 0;
			uint32_t start_offset = _offset;
			for (int32_t i = 0, len = s._children.size(); i < len; i++) {
				uint32_t child_size = s._children[i].size;
				buffer_sub_data(_offset, child_size, (char*)data + struct_offset);
				struct_offset += child_size;
				_offset += s._children[i].aligned_size;

				if (!_did_output)
					std::cout << "Buffered struct element size of: " << child_size << " aligment: "
						<< s._children[i].aligned_size << " offset: " << _offset << '\n';
			}
			if (!_did_output)
				std::cout << "Added struct padding size of: " << s.size() - (_offset - start_offset) << '\n';
			_offset = start_offset + s.size();
		}

		template<typename T, int Len>
		__ENABLE_IF_T_NOT_UBO_STRUCT_RET_VOID(T) buffer_block_array(T* first_element) {
			auto t = TID(T);
			if (uniformbuffer_struct::_base_aligments.contains(t)) {
				uint32_t ats = utilities::round_to(uniformbuffer_struct::_base_aligments[t], 16);
				for (int32_t i = 0; i < Len; i++) {
					buffer_sub_data(_offset, sizeof(T), first_element + i);
					_offset += ats;
					if (!_did_output)
						std::cout << "Buffered array element size of: " << sizeof(T) << " aligment: "
							<< ats << " offset: " << _offset << '\n';
				}
			}
			else {
				THROW_ERROR("Error to find 'base aligment' for type: " + t.name());
			}
		}

		template<typename T>
		void buffer_block_struct_array(uniformbuffer_struct& s, T* first_element, int Len) {
			for (int32_t i = 0; i < Len; i++) {
				buffer_block_struct(s, first_element + i);
			}
		}

	private:
		uint32_t _offset = 0;
		bool _did_output = false;
	};

}