#include "scene_serializer.h"

#include <string>
#include <vector>

#include "files.h"
#include "../../logging/logging.h"
#include "../../fng.h"

namespace feng::data {

	void scene_serializer::serialize(feng::scene* scene, std::filesystem::path path) {
		wfile file(path);
		file.write_raw(scene->get_uuid());
		file.write_raw(scene->_models.size());
		for (const auto& model : scene->_models)
			file.write_raw(model->get_uuid());
		file.write_raw(scene->_instances.size());
		for (const auto& instance : scene->_instances)
			file.write_serializable(instance.get());
	}

	void scene_serializer::serialize_models(feng::scene* scene, std::filesystem::path path) {
		wfile file(path);
		file.write_raw(scene->_models.size());
		for (const auto& model : scene->_models)
			file.write_serializable(model.get());
	}

	void scene_serializer::deserialize(feng::scene* scene, std::filesystem::path path) {
		LOG_ACTION("Deserialization of " + path.string() + " started.");
		rfile file(path);
		uuid_type uuid;
		file.read_raw(&uuid);
		scene->reset(uuid);
		size_t size;
		file.read_raw(&size);
		std::vector<uuid_type> models_uuids(size);
		file.read_raw((char*)models_uuids.data(), size * sizeof(uuid_type));
		file.read_raw(&size);
		scene->_instances.reserve(size);
		for (size_t i = 0; i < size; i++) {
			sptr_ins instance = std::make_shared<feng::instance>();
			file.read_serializable(instance.get(), scene);
			scene->_instances.emplace_back(instance);
		}
		LOG_INFO("Loaded " + std::to_string(size) + " instances.");
		LOG_ACTION("Deserialization of " + path.string() + " ended.");
	}

	void scene_serializer::deserialize_models(feng::scene* scene, std::filesystem::path path) {
		LOG_ACTION("Deserialization of " + path.string() + " started.");
		rfile file(path);
		size_t size;
		file.read_raw(&size);
		for (size_t i = 0; i < size; i++) {
			sptr_mdl model = std::make_shared<feng::model>();
			file.read_serializable(model.get(), scene);
			scene->register_model(model);
		}
		LOG_ACTION("Deserialization of " + path.string() + " ended.");
	}

	void scene_serializer::deserialize_baked_light(feng::scene* scene, std::filesystem::path path) {
		LOG_ACTION("Deserialization of " + path.string() + " started.");
		rfile file(path);
		uuid_type scene_uuid;
		file.read_raw(&scene_uuid);
		if (scene->get_uuid() != scene_uuid)
			LOG_WARNING("Data in '" + path.string() + "' may belong to another scene.");
		size_t no_textures;
		file.read_raw(&no_textures);
		LOG_INFO(std::to_string(no_textures));

		LOG_ACTION("Deserialization of " + path.string() + " ended.");
	}

}
