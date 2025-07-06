#include "box_renderer.h"

#include "glm/gtc/matrix_inverse.hpp"
#include "../../logging/logging.h"

namespace feng::helpers {

    // ---- box_renderer_base ----

    box_renderer_base::box_renderer_base(shader* shader, const std::array<glm::vec3, 8>& corners)
        : _shader(shader) { 
        generate_buffer_data(corners);
    }

    void box_renderer_base::generate_buffer_data(const std::array<glm::vec3, 8>& c) {
        _vertex_pos = c;
        _indices = {
            0, 1, 1, 3, 3, 2, 2, 0,
            4, 5, 5, 7, 7, 6, 6, 4,
            0, 4, 1, 5, 2, 6, 3, 7
        };
    }

    void box_renderer_base::start_allocation() {
        _VAO.generate();
        _VAO.bind();

        _EBO.generate();
        _EBO.bind();
        _EBO.buffer_data(_indices.size() * sizeof(uint32_t), _indices.data(), GL_STATIC_DRAW);

        _VBO.generate();
        _VBO.bind();
        _VBO.buffer_data(_vertex_pos.size() * sizeof(glm::vec3), _vertex_pos.data(), GL_STATIC_DRAW);
        _VAO.set_attrib_pointer(0, 3, GL_FLOAT, false, sizeof(glm::vec3), 0);
    }

    void box_renderer_base::setup_shader(const glm::vec3& color, const glm::mat4& model, const glm::mat4& view, 
        const glm::mat4& proj) {
        _shader->activate();
        _shader->set_mat4("model", model);
        _shader->set_mat4("view", view);
        _shader->set_mat4("proj", proj);
        _shader->set_3float("color", color);
    }

    std::array<glm::vec3, 8> box_renderer_base::corners_from_aabb(const feng::aabb& bounds) {
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

    std::array<glm::vec3, 8> box_renderer_base::compute_corners_from_matrix(const glm::mat4& m) {
        glm::mat4 inv = glm::inverse(m);
        std::array<glm::vec3, 8> corners;
        int idx = 0;
        for (int x = 0; x <= 1; ++x)
            for (int y = 0; y <= 1; ++y)
                for (int z = 0; z <= 1; ++z) {
                    glm::vec4 ndc(
                        x == 0 ? -1.f : 1.f,
                        y == 0 ? -1.f : 1.f,
                        z == 0 ? -1.f : 1.f,
                        1.f
                    );
                    glm::vec4 world = inv * ndc;
                    corners[idx++] = glm::vec3(world) / world.w;
                }
        return corners;
    }

    // ---- box_renderer ----

    box_renderer::box_renderer(shader* shader, const std::array<glm::vec3, 8>& corners) 
        : box_renderer_base(shader, corners) {
        start_allocation();
        vertexarray::unbind();
    }

    box_renderer::box_renderer(shader* shader, const glm::mat4& lightspace_matrix)
        : box_renderer(shader, compute_corners_from_matrix(lightspace_matrix)) {
    }

    box_renderer::box_renderer(shader* shader, const feng::aabb& bounds)
        : box_renderer(shader, corners_from_aabb(bounds)) {
    }

    void box_renderer::render(const glm::vec3& color, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {
        setup_shader(color, model, view, proj);

        _VAO.bind();
        _VAO.draw_elements(GL_LINES, _indices.size());
        vertexarray::unbind();
    }

    // ---- box_renderer_instanced ----

    box_renderer_instanced::box_renderer_instanced(shader* shader, const std::array<glm::vec3, 8>& corners, size_t max_no_instances)
        : box_renderer_base(shader, corners), _max_no_instances(max_no_instances) {
        allocate_buffers();
    }


    box_renderer_instanced::box_renderer_instanced(shader* shader, const feng::aabb& bounds, size_t max_no_instances)
        : box_renderer_instanced(shader, corners_from_aabb(bounds), max_no_instances) {
    }
    
    void box_renderer_instanced::allocate_buffers() {
        start_allocation();

        _pos_buffer.generate();
        _pos_buffer.bind();
        _pos_buffer.buffer_data<glm::vec3>(_max_no_instances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
        _VAO.set_attrib_pointer(1, 3, GL_FLOAT, false, sizeof(glm::vec3), 0, 1);

        _size_buffer.generate();
        _size_buffer.bind();
        _size_buffer.buffer_data<glm::vec3>(_max_no_instances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
        _VAO.set_attrib_pointer(2, 3, GL_FLOAT, false, sizeof(glm::vec3), 0, 1);

        vertexarray::unbind();
    }


    void box_renderer_instanced::add_instance(const feng::aabb& bounds) {
        _positions.push_back(bounds.center());
        _sizes.push_back(bounds.size());
    }

    void box_renderer_instanced::clear_instances() {
        _positions.clear();
        _sizes.clear();
    }

    void box_renderer_instanced::update_buffers() {
        uint32_t no_instances = get_no_instances();
        if (no_instances > 0) {
            _pos_buffer.bind();
            _pos_buffer.buffer_sub_data(0, no_instances * sizeof(glm::vec3), &_positions[0]);

            _size_buffer.bind();
            _size_buffer.buffer_sub_data(0, no_instances * sizeof(glm::vec3), &_sizes[0]);
        }
    }

    void box_renderer_instanced::render(const glm::vec3& color, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) {
        int32_t no_instances = get_no_instances();
        if (no_instances > 0) {
            setup_shader(color, model, view, proj);

            _VAO.bind();
            _VAO.draw_elements_instanced(GL_LINES, _indices.size(), GL_UNSIGNED_INT, 0, no_instances);
            vertexarray::unbind();
        }
    }

    size_t box_renderer_instanced::get_no_instances() const {
        return std::min(_positions.size(), _max_no_instances);
    }

}
