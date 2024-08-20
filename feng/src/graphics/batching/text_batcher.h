#pragma once

#include <vector>

#include "../text/text_renderer.h"

namespace feng {

	class text_batch {
	public:
		text_batch();
		~text_batch();

		std::vector<text_vertex> data;
		std::vector<uint32_t> textures_ids;

		void add_data_to_batch(std::vector<text_vertex>& data, uint32_t texture_id);
		void render(shader& shader);

	private:
		vertexarray vertex_array;
		arraybuffer vertex_buffer;

	}; 

	class text_batcher {
	public:
		text_batcher();

		static unsigned short max_no_characters_in_line;
		static unsigned char max_no_texture_units;
		
		static unsigned int get_max_buffer_len();

		void add_data_to_batcher(std::vector<text_vertex>& data, uint32_t texture_id);
		void render(shader& shader);
		void clear_bathces();

	private:
		std::vector<text_batch> _batches;

	};

}