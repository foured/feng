#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../shader.h"
#include "../vertexarray.h"
#include "../gl_buffers/arraybuffer.hpp"


namespace feng::helpers {

	class line_renderer {
	public:
		line_renderer(shader* shader, const std::vector<glm::vec3>& points);

		void render(const glm::vec3& color, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);

	private:
		std::vector<glm::vec3> _points;
		shader* _shader;
		vertexarray _array_object;
		arraybuffer _buffer_object;

	};

}