#include "text_renderer.h"

#include <glad/glad.h>
#include <string>

#include "../../logging/logging.h"
#include "../batching/text_batcher.h"

namespace feng {

	text_renderer::text_renderer(font_atlas& atlas) 
		: _atlas(atlas){

	}

	void text_renderer::render(text_batcher* batcher, const std::string& text, float x, float y, glm::vec3 color) {
		_vertices = std::vector<text_vertex>(text.size() * 6);
		float sx = 1, sy = 1;
		int n = 0;
		for (const char c : text) {
			character_info cci = _atlas.characters[c];

			float x2 = x + cci.bl * sx;
			float y2 = -y - cci.bt * sy;
			float w = cci.bw * sx;
			float h = cci.bh * sy;

			x += cci.ax * sx;
			y += cci.ay * sy;

			if (!w || !h)
				continue;

			float tx_left = cci.tx;
			float tx_right = cci.tx + cci.bw / _atlas.atlas_width;
			float ty_top = 0.0f;
			float ty_bottom = cci.bh / _atlas.atlas_height;

			_vertices[n++] = { x2,     -y2,     0, tx_left,  ty_top,    0, color };
			_vertices[n++] = { x2,     -y2 - h, 0, tx_left,  ty_bottom, 0, color };
			_vertices[n++] = { x2 + w, -y2,     0, tx_right, ty_top,    0, color };
			_vertices[n++] = { x2 + w, -y2,     0, tx_right, ty_top,    0, color };
			_vertices[n++] = { x2,     -y2 - h, 0, tx_left,  ty_bottom, 0, color };
			_vertices[n++] = { x2 + w, -y2 - h, 0, tx_right, ty_bottom, 0, color };
		}

		batcher->add_data_to_batcher(_vertices, _atlas.texture_id);
	}

}