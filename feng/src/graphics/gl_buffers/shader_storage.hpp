#pragma once

#include <unordered_map>

#include "gl_buffer.hpp"
#include "../../logging/logging.h"
#include "../../fng.h"
#include "glstandards.h"

/*
	SSBO:
		You have to align offset for each element to its 'base alignment'
		You have to align structures to 'base alignment' of vec4 (16)

*/

namespace feng {

	class shader_storage : public gl_buffer {
	public:
		shader_storage(glstd::buffer_std buffer_type)
			: gl_buffer(get_buffer_define(buffer_type), false),
			_standard(buffer_type) { 
		}

		void allocate(uint32_t data_size, const std::string& name) {
			uint32_t binding_slot = get_name_binding(name);
			generate();
			bind();
			buffer_data<uint32_t>(data_size, NULL, GL_DYNAMIC_DRAW);
			bind_buffer_base(binding_slot);
			unbind();
		}

		void allocate(const glstd::buffer_structure& final_buf_strct, const std::string& name) {
			allocate(final_buf_strct.size(_standard == glstd::buffer_std::std430), name);
		}

		void start_block() {
			bind();
			_offset = 0;
		}

		void rount_to_vec4() {
			_offset = utilities::round_to(_offset, 16);
		}

		template<typename T>
		void add_element(T* data) {
			IS_ALIGNMENTABLE(T);
			uint32_t alignment = BASE_ALIGNMENT(T);
			if (_array_mode) {
				alignment = utilities::round_to(alignment, 16);
			}
			buffer_and_offset_sub_data(TS(T), alignment, (void*)data);
		}

		void add_structure(const glstd::buffer_structure& bs, void* data) {
			uint32_t struct_offset = 0;
			for (const glstd::buffer_element e : bs.elements) {
				if (!e.is_empty) {
					buffer_and_offset_sub_data(e.size, e.aligned_size, (char*)data + struct_offset);
				}
				else {
					LOG_WARNING("Untested part of code was used xD (ssbo->add_structure (empty element))");
				}
				struct_offset += e.size;
			}
			rount_to_vec4();
		}

		void end_block() {
			unbind();
		}

		void start_array() {
#ifdef FENG_DEBUG
			if (_standard == glstd::buffer_std::std430) {
				LOG_WARNING("No need to use 'start_array' in shader storage for std430, it is for std140");
				return;
			}
#endif
			_array_mode = true;
		}

		void end_array() {
#ifdef FENG_DEBUG
			if (_standard == glstd::buffer_std::std430) {
				LOG_WARNING("No need to use 'end_array' in shader storage for std430, it is for std140");
				return;
			}
#endif
			_array_mode = false;
		}

		static inline const glstd::buffer_structure dirlight_buffer_structure 
			= glstd::buffer_structure::make_buffer_structure<glm::vec3, glm::vec3, glm::vec3, glm::vec3>();

		static inline const glstd::buffer_structure spotlight_buffer_structure
			= glstd::buffer_structure::make_buffer_structure<glm::vec3, glm::vec3, float, float, float, float, float, glm::vec3, glm::vec3, glm::vec3>();

		static inline const glstd::buffer_structure pointlight_buffer_structure
			= glstd::buffer_structure::make_buffer_structure<glm::vec3, float, float, float, glm::vec3, glm::vec3, glm::vec3, float>();

		static glstd::buffer_structure generate_lights_buffer() {
			glstd::buffer_structure lighs_final_buffer_structure;
			lighs_final_buffer_structure.add_struct(dirlight_buffer_structure);
			lighs_final_buffer_structure.add_element<int>();
			for(int32_t i = 0; i < MAX_SPOT_LIGHTS; i++)
				lighs_final_buffer_structure.add_struct(spotlight_buffer_structure);
			lighs_final_buffer_structure.add_element<int>();
			for (int32_t i = 0; i < MAX_POINT_LIGHTS; i++)
				lighs_final_buffer_structure.add_struct(pointlight_buffer_structure);

			return lighs_final_buffer_structure;
		}

		static void add_name_binding(const std::string& name, uint32_t slot) {
			FENG_ASSERT(!_buffers_binds.contains(name), "Buffer block name: '", name, "' already exists.");
			_buffers_binds[name] = slot;
		}

		static uint32_t get_name_binding(const std::string& name) {
			FENG_ASSERT(_buffers_binds.contains(name), "Buffer block name: '", name, "' doesnt exists. Add it with \
				shader_storage::add_name_binding first.");
			return _buffers_binds[name];
		}

	private:
		uint32_t _offset = 0;
		glstd::buffer_std _standard;
		bool _array_mode = false;

		static inline std::unordered_map<std::string, uint32_t> _buffers_binds {};

		void buffer_and_offset_sub_data(uint32_t size, uint32_t alignment, void* data) {
			_offset = utilities::round_to(_offset, alignment);
			buffer_sub_data(_offset, size, data);
			_offset += size;
		}

		static uint32_t get_buffer_define(glstd::buffer_std bstd) {
			if (bstd == glstd::buffer_std::std140) {
				return GL_UNIFORM_BUFFER;
			}
			return GL_SHADER_STORAGE_BUFFER;
		}
	};

}