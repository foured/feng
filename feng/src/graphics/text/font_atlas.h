#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  

#include <iostream>

namespace feng {

	struct character_info {
		float ax;
		float ay;

		float bw;
		float bh;

		float bl;
		float bt;

		float tx;
	};

	class font_atlas {
	public:
		font_atlas();
		font_atlas(const char* font_path, FT_Library& lib, uint32_t size);

		uint32_t texture_id;
		uint32_t atlas_width, atlas_height;
		character_info characters[128];

	};

}