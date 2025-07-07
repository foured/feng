#include "text_batcher.h"

#include "../renderer.h"

namespace feng {

	unsigned short text_batcher::max_no_characters_in_line = 100;

	text_batcher::text_batcher() {
		_batches.emplace_back();
	}

	text_batch::text_batch() {
		vertex_array.generate();
		vertex_array.bind();
		vertex_buffer.generate();
		vertex_buffer.bind();

		vertex_buffer.buffer_data<float>(
			sizeof(text_vertex) * text_batcher::get_max_buffer_len(),
			NULL,
			GL_DYNAMIC_DRAW);

		vertex_array.set_attrib_pointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(text_vertex), 0);
		vertex_array.set_attrib_pointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(text_vertex), (3 * sizeof(float)));
		vertex_array.set_attrib_pointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(text_vertex), (5 * sizeof(float)));
		vertex_array.set_attrib_pointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(text_vertex), (6 * sizeof(float)));

		vertex_buffer.unbind();
		vertexarray::unbind();
	}

	text_batch::~text_batch() {
		vertex_array.free();
		vertex_buffer.free();
	}

	void text_batcher::add_data_to_batcher(std::vector<text_vertex>& data, uint32_t texture_id) {
		bool setup = false;
		for (text_batch& batch : _batches) {
			if (batch.textures_ids.size() < renderer::get_max_no_texture_units() &&
				data.size() <= get_max_buffer_len() - batch.data.size())
			{
				setup = true;
				batch.add_data_to_batch(data, texture_id);
				break;
			}
		}
		if (!setup) {
			_batches.emplace_back();
			_batches.back().add_data_to_batch(data, texture_id);
		}
	}

	void text_batcher::render(shader& shader) {
		for (text_batch& batch : _batches) {
			batch.render(shader);
		}
		clear_bathces();
	}

	void text_batch::render(shader& shader) {
		for (uint8_t i = 0; i < textures_ids.size(); i++) {
			shader.set_int("textures[" + std::to_string(i) + "]", i);
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures_ids[i]);
		}

		vertex_buffer.bind();
		vertex_buffer.buffer_sub_data(0, sizeof(text_vertex) * data.size(), data.data());
		vertex_array.bind();
		vertex_array.draw_arrays(GL_TRIANGLES, data.size());

		vertexarray::unbind();
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void text_batch::add_data_to_batch(std::vector<text_vertex>& ndata, uint32_t texture_id) {
		uint32_t si = data.size();
		data.insert(data.end(), ndata.begin(), ndata.end());
		for (uint32_t i = si; i < data.size(); i++)
			data[i].texture_index = (int)textures_ids.size();;
		textures_ids.push_back(texture_id);
	}

	unsigned int text_batcher::get_max_buffer_len() {
		return 6 * text_batcher::max_no_characters_in_line * renderer::get_max_no_texture_units();
	}

	void text_batcher::clear_bathces() {
		for (text_batch& batch : _batches) {
			batch.data.clear();
			batch.textures_ids.clear();
		}
	}

}