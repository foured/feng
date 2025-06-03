#include "lightbaker.h"

namespace feng::editor {

	void lightbaker::bake(scene* scene, const std::filesystem::path& path) {
		for (auto instance : scene->_instances) {
			if (instance.get()->flags.get(INST_FLAG_STATIC) and instance.get()->flags.get(INST_FLAG_RCV_SHADOWS)) {

			}
		}
	}

}