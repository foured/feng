#pragma once

#include "../tag.h"

#include "../../../graphics/texture.h"

namespace feng {

	class shadow_texture_owner : public tag {
	public:
		shadow_texture_owner();
		~shadow_texture_owner();

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		texture _shadow_texture;

	};
}