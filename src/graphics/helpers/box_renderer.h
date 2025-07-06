#pragma once

#include <array>
#include <vector>
#include "glm/glm.hpp"

#include "../vertexarray.h"
#include "../gl_buffers/arraybuffer.hpp"
#include "../gl_buffers/elementarraybuffer.hpp"
#include "../../algorithms/aabb.h"
#include "../shader.h"

namespace feng::helpers {

    class box_renderer_base {
    protected:
        box_renderer_base(shader* shader, const std::array<glm::vec3, 8>& corners);

        std::array<glm::vec3, 8> _vertex_pos;
        std::array<uint32_t, 24> _indices;

        vertexarray _VAO;
        arraybuffer _VBO;
        elementarraybuffer _EBO;

        shader* _shader;

        void start_allocation();
        void setup_shader(const glm::vec3& color, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);

        static std::array<glm::vec3, 8> corners_from_aabb(const feng::aabb& bounds);
        static std::array<glm::vec3, 8> compute_corners_from_matrix(const glm::mat4& m);

    private:
        void generate_buffer_data(const std::array<glm::vec3, 8>& corners);

    };

    class box_renderer : public box_renderer_base {
    public:
        box_renderer(shader* shader, const std::array<glm::vec3, 8>& corners);
        box_renderer(shader* shader, const glm::mat4& lightspace_matrix);
        box_renderer(shader* shader, const feng::aabb& bounds);

        void render(const glm::vec3& color, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);

    };

    class box_renderer_instanced : public box_renderer_base {
    public:
        box_renderer_instanced(shader* shader, const std::array<glm::vec3, 8>& corners, size_t max_no_instances = 10);
        box_renderer_instanced(shader* shader, const feng::aabb& bounds, size_t max_no_instances = 10);

        void add_instance(const aabb& bounds);
        void clear_instances();
        void update_buffers();
        void render(const glm::vec3& color, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);

    private:
        size_t _max_no_instances;
        arraybuffer _pos_buffer;
        arraybuffer _size_buffer;
        std::vector<glm::vec3> _positions;
        std::vector<glm::vec3> _sizes;

        void allocate_buffers();
        size_t get_no_instances() const;
    };

}
