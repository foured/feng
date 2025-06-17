#pragma once

#include <vector>
#include "model.h"

namespace feng {

    const std::vector<vertex> cube_vertices{
        { glm::vec3(1, 1, -1), glm::vec3(0, 1, 0), glm::vec2(0.625, 0.5), glm::vec3(-1, 0, 0)},
        { glm::vec3(-1, 1, -1), glm::vec3(0, 1, 0), glm::vec2(0.875, 0.5), glm::vec3(-1, 0, 0)},
        { glm::vec3(-1, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0.875, 0.25), glm::vec3(-1, 0, 0)},
        { glm::vec3(1, 1, 1), glm::vec3(0, 1, 0), glm::vec2(0.625, 0.25), glm::vec3(-1, 0, 0)},
        { glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(0.375, 0.25), glm::vec3(0, 1, 0)},
        { glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0.625, 0.25), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, 1, 1), glm::vec3(0, 0, 1), glm::vec2(0.625, 0), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), glm::vec2(0.375, 0), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, -1, 1), glm::vec3(-1, 0, 0), glm::vec2(0.375, 1), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, 1, 1), glm::vec3(-1, 0, 0), glm::vec2(0.625, 1), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, 1, -1), glm::vec3(-1, 0, 0), glm::vec2(0.625, 0.75), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, -1, -1), glm::vec3(-1, 0, 0), glm::vec2(0.375, 0.75), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(0.125, 0.5), glm::vec3(1, 0, 0)},
        { glm::vec3(1, -1, -1), glm::vec3(0, -1, 0), glm::vec2(0.375, 0.5), glm::vec3(1, 0, 0)},
        { glm::vec3(1, -1, 1), glm::vec3(0, -1, 0), glm::vec2(0.375, 0.25), glm::vec3(1, 0, 0)},
        { glm::vec3(-1, -1, 1), glm::vec3(0, -1, 0), glm::vec2(0.125, 0.25), glm::vec3(1, 0, 0)},
        { glm::vec3(1, -1, -1), glm::vec3(1, 0, 0), glm::vec2(0.375, 0.5), glm::vec3(0, 1, 0)},
        { glm::vec3(1, 1, -1), glm::vec3(1, 0, 0), glm::vec2(0.625, 0.5), glm::vec3(0, 1, 0)},
        { glm::vec3(1, 1, 1), glm::vec3(1, 0, 0), glm::vec2(0.625, 0.25), glm::vec3(0, 1, 0)},
        { glm::vec3(1, -1, 1), glm::vec3(1, 0, 0), glm::vec2(0.375, 0.25), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(0.375, 0.75), glm::vec3(0, 1, 0)},
        { glm::vec3(-1, 1, -1), glm::vec3(0, 0, -1), glm::vec2(0.625, 0.75), glm::vec3(0, 1, 0)},
        { glm::vec3(1, 1, -1), glm::vec3(0, 0, -1), glm::vec2(0.625, 0.5), glm::vec3(0, 1, 0)},
        { glm::vec3(1, -1, -1), glm::vec3(0, 0, -1), glm::vec2(0.375, 0.5), glm::vec3(0, 1, 0)}
    };

    const std::vector<uint32_t> cube_indices{
        0, 1, 2, 
        0, 2, 3, 
        4, 5, 6, 
        4, 6, 7, 
        8, 9, 10, 
        8, 10, 11, 
        12, 13, 14, 
        12, 14, 15, 
        16, 17, 18, 
        16, 18, 19, 
        20, 21, 22, 
        20, 22, 23
    };

    const std::vector<vertex> plane_vertices{
        { glm::vec3(-1, 0, -1),  glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(-1, 0, 1),  glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(1, 0, 1),  glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(1.0f, 0.0f, 0.0f) },
        { glm::vec3(1, 0, -1),  glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(1.0f, 0.0f, 0.0f) },
    };

    const std::vector<uint32_t> plane_indices{
        0, 1, 2,
        2, 3, 0
    };

	struct primitives {
        static std::vector<mesh> generate_cube_mesh(std::vector<texture> textures);
        static std::vector<mesh> generate_cube_mesh(glm::vec3 diffuse, glm::vec3 specular);

        static std::vector<mesh> generate_plane_mesh(std::vector<texture> textures);
        static std::vector<mesh> generate_plane_mesh(glm::vec3 diffuse, glm::vec3 specular);
	};

}