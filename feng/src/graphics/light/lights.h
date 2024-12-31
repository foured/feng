#pragma once

#include <glm/glm.hpp>

#include "../../fng.h"
#include "../gl_buffers/framebuffer.hpp"
#include "../texture.h"

namespace feng {

    class DirLight {
    public:
        DirLight(uint32_t shadowmap_size = SHADOWMAP_SIZE);
        DirLight(glm::vec3 dir, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, uint32_t shadowmap_size = SHADOWMAP_SIZE);

        glm::vec3 direction;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;


        glm::mat4 generate_lightspace_matrix();
        void generate_buffers();
        void render_preparations();
        void render_cleanup();
        void bind_shadowmap(uint32_t slot = SHADOWMAP_TEXTURE_SLOT);

    private:
        uint32_t _shadowmap_size;
        framebuffer _depthmap_framebuffer;
        texture _shadowmap;
    };

    struct PointLight {
        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
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