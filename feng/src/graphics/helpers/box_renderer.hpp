#pragma once

#include <array>

#include "glm/glm.hpp"

#include "../vertexarray.h"
#include "../gl_buffers/arraybuffer.hpp"

#include "../shader.h"

namespace feng::helpers {

	class box_renderer {
	public:
        box_renderer(shader* shader, const std::array<glm::vec3, 8>& corners) 
            : _shader(shader) {
            generate_lines(corners);
            setup_buffers();
		}

        box_renderer(shader* shader, const glm::mat4& lightspace_matrix)
            : box_renderer(shader, compute_corners_from_matrix(lightspace_matrix)) { }

        box_renderer(shader* shader, const feng::aabb& bounds)
            : box_renderer(shader, corners_from_aabb(bounds)) { }

        void render(const glm::vec3& color, const glm::mat4& view, const glm::mat4& proj) {
            _shader->activate();
            _shader->set_mat4("view", view);
            _shader->set_mat4("proj", proj);
            _shader->set_3float("color", color);

            _VAO.bind();
            _VAO.draw_arrays(GL_LINES, _lines.size());

            vertexarray::unbind();
        }

	private:
		std::array<glm::vec3, 24> _lines;

        vertexarray _VAO;
        arraybuffer _VBO;

        shader* _shader;

		void generate_lines(const std::array<glm::vec3, 8>& c) {
            _lines = {
                // Нижняя грань
                c[0], c[1],
                c[1], c[3],
                c[3], c[2],
                c[2], c[0],

                // Верхняя грань
                c[4], c[5],
                c[5], c[7],
                c[7], c[6],
                c[6], c[4],

                // Боковые рёбра
                c[0], c[4],
                c[1], c[5],
                c[2], c[6],
                c[3], c[7],
            };
		}

        void setup_buffers() {
            _VAO.generate();
            _VAO.bind();

            _VBO.generate();
            _VBO.bind();

            _VAO.set_attrib_pointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);

            _VBO.buffer_data(_lines.size() * sizeof(glm::vec3), _lines.data(), GL_DYNAMIC_DRAW);

            vertexarray::unbind();
        }

        static std::array<glm::vec3, 8> compute_corners_from_matrix(const glm::mat4& m) {
            glm::mat4 inv = glm::inverse(m);
            std::array<glm::vec3, 8> corners;
            int idx = 0;
            for (int x = 0; x <= 1; ++x)
                for (int y = 0; y <= 1; ++y)
                    for (int z = 0; z <= 1; ++z) {
                        glm::vec4 ndc(
                            x == 0 ? -1.0f : 1.0f,
                            y == 0 ? -1.0f : 1.0f,
                            z == 0 ? -1.0f : 1.0f,
                            1.0f
                        );
                        glm::vec4 world = inv * ndc;
                        corners[idx++] = glm::vec3(world) / world.w;
                    }
            return corners;
        }

        static std::array<glm::vec3, 8> corners_from_aabb(const feng::aabb& bounds) {
            const glm::vec3& min = bounds.min;
            const glm::vec3& max = bounds.max;

            return {
                glm::vec3(min.x, min.y, min.z),
                glm::vec3(min.x, min.y, max.z),
                glm::vec3(min.x, max.y, min.z),
                glm::vec3(min.x, max.y, max.z),
                glm::vec3(max.x, min.y, min.z),
                glm::vec3(max.x, min.y, max.z),
                glm::vec3(max.x, max.y, min.z),
                glm::vec3(max.x, max.y, max.z),
            };
        }


	};

}