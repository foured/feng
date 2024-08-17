#pragma once

#include <vector>
#include <string>

namespace feng {

	class cubemap {
	public:
		cubemap(const std::vector<std::string>& faces);

		uint32_t id();
		void bind();

	private:
		uint32_t _texture_id;
	};

}