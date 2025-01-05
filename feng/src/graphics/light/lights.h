#pragma once

#include <glm/glm.hpp>
#include <array>

#include "../../fng.h"
#include "../gl_buffers/framebuffer.hpp"
#include "../cubemap.h"
#include "../texture.h"

namespace feng {

    class DirLight {
    public:
        DirLight(uint32_t shadowmap_size = SHADOWMAP_SIZE);
        DirLight(
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

        void generate_lightspace_matrix();
        void generate_buffers();
        void render_preparations();
        void render_cleanup();
        void bind_shadowmap(uint32_t slot = SHADOWMAP_TEXTURE_SLOT);

    private:
        uint32_t _shadowmap_size;
        framebuffer _depthmap_framebuffer;
        texture _shadowmap;
    };

    class PointLight {
    public:
        PointLight(uint32_t shadowmap_size = SHADOWMAP_SIZE);
        PointLight(
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

    struct SpotLight {
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