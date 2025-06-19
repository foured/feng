#include "vertexarray.h"

namespace feng {

	void vertexarray::free() {
		glDeleteVertexArrays(1, &_id);
	}

	void vertexarray::generate() {
		glGenVertexArrays(1, &_id);
	}

	void vertexarray::bind() {
		glBindVertexArray(_id);
	}

	void vertexarray::set_attrib_pointer(uint32_t index, uint32_t no_data_elements, GLenum element_type,
		bool triangulate, uint32_t data_size, uint32_t offset, int32_t devisor) {
		glVertexAttribPointer(index, no_data_elements, element_type, triangulate, data_size, (void*)offset);
		glEnableVertexAttribArray(index);

		if (devisor != 0)
			glVertexAttribDivisor(index, devisor);
	}

	void vertexarray::draw_elements(int32_t mode, uint32_t no_indices) {
		glDrawElements(mode, no_indices, GL_UNSIGNED_INT, 0);
	}

	void vertexarray::draw_arrays(int32_t mode, uint32_t no_vertices) {
		glDrawArrays(mode, 0, no_vertices);
	}

	void vertexarray::draw_elements_instanced(int32_t mode, int32_t count, int32_t type,
		const void* indices, int32_t primcount) {
		glDrawElementsInstanced(mode, count, type, indices, primcount);
	}

	void vertexarray::unbind() {
		glBindVertexArray(0);
	}

}