#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace feng {
	class vertexarray {
	public:
		void generate();
		void bind();
		void set_attrib_pointer(uint32_t index, uint32_t no_elements, GLenum element_type,
			bool triangulate, uint32_t data_size, uint32_t offset, int32_t devisor = 0);
		void draw_elements(int32_t mode, uint32_t no_indices);
		void draw_arrays(int32_t mode, uint32_t no_vertices);
		void draw_elements_inctanced(int32_t mode, int32_t count, int32_t type, 
			const void *indices, int32_t primcount);

		static void unbind();

	private:
		uint32_t _id;

	};
}