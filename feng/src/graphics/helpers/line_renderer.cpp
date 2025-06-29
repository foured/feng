#include "line_renderer.h"

namespace feng::helpers {

	line_renderer::line_renderer(shader* shader, const std::vector<glm::vec3>& points)
		: _shader(shader), _points(points) {
		_array_object.generate();
		_array_object.bind();

		_buffer_object.generate();
		_buffer_object.bind();

		_buffer_object.buffer_data(_points.size() * sizeof(glm::vec3), _points.data(), GL_STATIC_DRAW);
		_array_object.set_attrib_pointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);

		vertexarray::unbind();
	}

	void line_renderer::render(const glm::vec3& color, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {
		_shader->activate();
		_shader->set_mat4("model", model);
		_shader->set_mat4("view", view);
		_shader->set_mat4("proj", proj);
		_shader->set_3float("color", color);

		_array_object.bind();
		_array_object.draw_arrays(GL_LINES, _points.size());
		vertexarray::unbind();
	}

}