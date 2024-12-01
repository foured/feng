#pragma once

#include <vector>
#include <string>
#include <array>

namespace feng {

	class cubemap {
	public:
		cubemap(const std::array<std::string, 6>& faces);

		uint32_t id();
		void bind();

	private:
		uint32_t _texture_id;
	};

}