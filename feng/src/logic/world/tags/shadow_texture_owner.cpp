#include "shadow_texture_owner.h"

#include "../../data_management/files.h"

namespace feng {

	shadow_texture_owner::shadow_texture_owner() {}

	shadow_texture_owner::~shadow_texture_owner() {
		_shadow_texture.del();
	}

	void shadow_texture_owner::serialize(data::wfile* file) {
		file->write_serializable(&_shadow_texture);
	}

	void shadow_texture_owner::deserialize(data::rfile* file, scene* scene) {
		file->read_serializable(&_shadow_texture, scene);
	}

}