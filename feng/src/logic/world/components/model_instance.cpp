#include "model_instance.h"

#include "../instance.h"
#include "../../data_management/files.h"
#include "../scene.h"

namespace feng {

	model_instance::model_instance(instance* instance, std::shared_ptr<model> model)
		: component(instance), _model(model) {}

	void model_instance::start() {

	}

	void model_instance::update() {
		_model.get()->add_instance(_instance);
	}

	std::shared_ptr<component> model_instance::copy(instance* new_instance) {
		return std::make_shared<model_instance>(new_instance, _model);
	}

	void model_instance::serialize(data::wfile* file)
	{
		component_list type = component_list::model_instance;
		file->write_raw(type);
		file->write_raw(_model->get_uuid());
	}

	void model_instance::deserialize(data::rfile* file, scene* scene)
	{
		uuid_type model_uuid;
		file->read_raw(&model_uuid);
		_model = scene->find_model(model_uuid);
	}

	void model_instance::render_alone(shader& shader) {
		_model->render_single_instance(shader, _instance);
	}

	aabb model_instance::calculate_bounds() {
		aabb* mb = &_model->bounds;
		transform* t = &_instance->transform;
		glm::vec3 max = mb->max * t->get_size() + t->get_position();
		glm::vec3 min = mb->min * t->get_size() + t->get_position();
		return aabb(max, min);
	}

	std::shared_ptr<model> model_instance::get_model() const
	{
		return _model;
	}

}