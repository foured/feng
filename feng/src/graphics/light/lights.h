#pragma once

#include <glm/glm.hpp>
#include <array>

#include "../../fng.h"
#include "../gl_buffers/framebuffer.hpp"
#include "../cubemap.h"
#include "../texture.h"
#include "../../logic/aabb.h"

namespace feng {

    class dir_light {
    public:
        dir_light(uint32_t shadowmap_size = SHADOWMAP_SIZE);
        dir_light(
            const glm::vec3& dir, 
            const glm::vec3& ambient, 
            const glm::vec3& diffuse, 
            const glm::vec3& specular, 
            uint32_t shadowmap_size = SHADOWMAP_SIZE);

        glm::vec3 direction;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        glm::mat4 lightspace_matrix;


        //===================
        // MATRICES
        //===================
        void generate_lightspace_matrix();
        glm::mat4 generate_custom_lightspace_matrix(const glm::vec3& min, const glm::vec3& max);
        glm::mat4 generate_custom_lightspace_matrix(const aabb& bounds);
        glm::mat4 generate_custom_relative_lightspace_matrix(const glm::vec3& cmin, const glm::vec3& cmax,
            const glm::vec3& rmin, const glm::vec3& rmax);
        glm::mat4 generate_custom_relative_lightspace_matrix(const aabb& caster, const aabb& receiver);

        void generate_buffers();
        void render_preparations();
        void full_render_preparations(shader& shader, glm::mat4 model);
        void render_cleanup();
        void bind_shadowmap(uint32_t slot = SHADOWMAP_TEXTURE_SLOT);

        void delete_buffers();

        texture get_texture() const;

        texture _shadowmap;

    private:
        uint32_t _shadowmap_size;
        framebuffer _depthmap_framebuffer;
    };

    class point_light {
    public:
        point_light(uint32_t shadowmap_size = SHADOWMAP_SIZE);
        point_light(
            const glm::vec3& pos, 
            float cons, 
            float lin, 
            float quad, 
            const glm::vec3& amb, 
            const glm::vec3& diff, 
            const glm::vec3& spec,
            uint32_t shadowmap_size = SHADOWMAP_SIZE);

        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

        float far_plane = 50.0f;

        std::array<glm::mat4, 6> lightspace_matrices;

        void generate_lightspace_matrices();
        void generate_buffers();
        void render_preparations(shader& shader);
        void render_cleanup();
        void bind_shadowmap(uint32_t slot);

    private:
        uint32_t _shadowmap_size;
        framebuffer _framebuffer;
        cubemap _shadowmap;
    };

    struct spot_light {
        glm::vec3 position;
        glm::vec3 direction;

        float cutOff;
        float outerCutOff;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

}