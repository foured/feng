#pragma once

#include "gl_buffer.hpp"
#include "../../logging/logging.h"
#include "../../fng.h"
#include "glstandards.h"

/*

	You have to align offset for each element to its 'base alignment'
	You have to align structures to 'base alignment' of vec4 (16)

*/

namespace feng {

	class ssbo : public gl_buffer {
	public:
		ssbo()
			: gl_buffer(GL_SHADER_STORAGE_BUFFER, false) { }

		void allocate(uint32_t data_size, uint32_t binding_slot) {
			generate();
			bind();
			buffer_data<uint32_t>(data_size, NULL, GL_DYNAMIC_DRAW);
			bind_buffer_base(binding_slot);
			unbind();
		}

		void allocate(const glstd::buffer_structure& final_buf_strct, uint32_t binding_slot) {
			allocate(final_buf_strct.size(), binding_slot);
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
			buffer_and_offset_sub_data(TS(T), glstd::base_aligments[TID(T)], (void*)data);
		}

		void add_structure(const glstd::buffer_structure& bs, void* data) {
			uint32_t struct_offset = 0;
			for (const glstd::buffer_element e : bs.elements) {
				if (!e.is_empty)
					buffer_and_offset_sub_data(e.size, e.aligned_size, (char*)data + struct_offset);
				else
					LOG_WARNING("Untested part of code was used xD (ssbo->add_structure (empty element))");
				struct_offset += e.size;
			}
			rount_to_vec4();
		}

		void end_block() {
			unbind();
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

	private:
		uint32_t _offset = 0;

		void buffer_and_offset_sub_data(uint32_t size, uint32_t alignment, void* data) {
			_offset = utilities::round_to(_offset, alignment);
			buffer_sub_data(_offset, size, data);
			_offset += size;
		}
	};

}