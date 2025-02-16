#include "scene_serializer.h"

#include "files.h"
#include "../../logging/logging.h"
#include "../../fng.h"

namespace feng::data {

	void scene_serializer::serialize(scene* scene, std::filesystem::path path) {
		wfile file(path);
		LOG_INFO(scene->get_uuid_string());
		file.write_raw(scene->get_uuid());
		file.write_raw(scene->_models.size());
		for (const auto& model : scene->_models)
			file.write_raw(model->get_uuid());
		file.write_raw(scene->_instances.size());
		for (const auto& instance : scene->_instances)
			file.write_serializable(instance.get());
	}

}
