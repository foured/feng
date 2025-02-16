#pragma once

//https://www.youtube.com/watch?v=NCptEJ1Uevg&t=331s&ab_channel=OGLDEV

#include <vector>

namespace feng {

	class shadowmap_offset_texture {
	public:
		shadowmap_offset_texture(uint32_t win_size, uint32_t filter_size);

		void bind();
		void bind_to_slot(uint32_t slot);

	private:
		uint32_t _id;
	};

}