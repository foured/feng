#include "renderer.h"

#include <glad/glad.h>

#include "shader.h"

namespace feng {
    int32_t renderer::_max_no_texture_units = -1;

    void renderer::get_static_values(){
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &_max_no_texture_units);
        shader::add_external_define("MAX_NO_TEXTURE_UNITS", std::to_string(_max_no_texture_units));
        shader::add_external_define("MAX_NO_FREE_TEXTURE_UNITS", std::to_string(get_max_no_free_texture_units()));
    }

    int32_t renderer::get_max_no_texture_units(){
        return _max_no_texture_units;
    }

    int32_t renderer::get_shadowmap_texture_slot(){
        return _max_no_texture_units - 1;
    }

    int32_t renderer::get_max_no_free_texture_units(){
        // -1 (shadowmap)
        return _max_no_texture_units - 1;
    }

}