#include "flash_light.h"

#include "../instance.h"
#include "../../data_management/files.h"
#include "../scene.h"

namespace feng {

	flash_light::flash_light(instance* instance) 
		: component(instance) {}

	flash_light::flash_light(instance* instance, scene* scene, 
		float cut_off, float outer_cut_off,
		float constant, float linear, float quadratic, 
		glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
		: component(instance), _scene(scene),
			_static {    
				cut_off,
				outer_cut_off,
				constant,
				linear,
				quadratic,
				ambient,
				diffuse,
				specular
			}
	{
		_idx = _scene->get_free_spot_light_idx();
	}

	flash_light::flash_light(instance* instance, scene* scene, static_data data) 
		:  component(instance), _scene(scene), _static(data) {
		_idx = _scene->get_free_spot_light_idx();
	}

	void flash_light::start() {
		update_dynamic_data();
		update_static_data();
	}

	void flash_light::late_start() {

	}

	void flash_light::update() {
		update_dynamic_data();
	}

	std::shared_ptr<component> flash_light::copy(instance* new_instance) {
		return std::make_shared<flash_light>(new_instance, _scene, _static);
	}

	void flash_light::serialize(data::wfile* file)
	{
		component_list type = component_list::flash_light;
		file->write_raw(type);
		file->write_raw(_static);
	}

	void flash_light::deserialize(data::rfile* file, scene* scene)
	{
		THROW_ERROR("Unimplemented code error.");
	}

	void flash_light::update_dynamic_data() {
		if (_idx != -1) {
			spot_light* sl = &_scene->spot_lights[_idx];
			sl->position = _scene->main_camera.position;
			sl->direction = _scene->main_camera.front();
		}
	}

	void flash_light::update_static_data() {
		if (_idx != -1) {
			spot_light* sl = &_scene->spot_lights[_idx];
			sl->cutOff = _static.cut_off;
			sl->outerCutOff = _static.outer_cut_off;
			sl->constant = _static.constant;
			sl->linear = _static.linear;
			sl->quadratic = _static.quadratic;
			sl->ambient = _static.ambient;
			sl->diffuse = _static.diffuse;
			sl->specular = _static.specular;
		}
	}

}