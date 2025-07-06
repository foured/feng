#ifdef IMPLEMENT_WORK_IN_PROGRESS_CODE

#pragma once

#include <array>

#include "texture.h"
#include "helpers/fullscreen_quad.hpp"
#include "gl_buffers/framebuffer.hpp"
#include "../utilities/singleton.hpp"

namespace feng {

	//class gaussian_blur : public util::singleton<gaussian_blur> {
	//public:
	//	void generate_buffers();
	//	void blur(texture& texture);

	//private:
	//	helpers::fullscreen_quad _fs_quad;
	//	//framebuffer _framebuffer;

	//};

    class gaussian_blur {
    public:
		gaussian_blur(helpers::fullscreen_quad* fs_quad, texture* input_texture);
        ~gaussian_blur();

		void draw(uint32_t iterations);

		texture& test_get_texture();

    private:
        helpers::fullscreen_quad* _fs_quad;
        shader* _shader;
		texture* _input_texture;
		std::array<framebuffer, 2> _framebuffers;
		std::array<texture, 2> _textures;

    };


}


#endif