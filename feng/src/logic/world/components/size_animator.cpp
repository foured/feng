#include "size_animator.h"

#include "../instance.h"
#include "../../data_management/files.h"
#include "../scene.h"

namespace feng {

	size_animator::size_animator(instance* instance, glm::vec3 start_size, glm::vec3 end_size, float speed)
		: _start(start_size), _end(end_size), _speed(speed), 
		_dir(glm::normalize(end_size - start_size)), component(instance) {
		_mtime = glm::length(_end - _start) / speed;

	}

	void size_animator::start() {
		_instance->transform.set_size(_start);
	}

	void size_animator::update() {
		float k = _fets ? -1 : 1;
		glm::vec3 d = _dir * _speed * (float)utilities::delta_time() * k;
		_instance->transform.set_size(_instance->transform.get_size() + d);
		_ctime += utilities::delta_time();
		if (_ctime >= _mtime) {
			_ctime = 0;
			_fets = !_fets;
		}
	}

	std::shared_ptr<component> size_animator::copy(instance* new_instance) {
		return std::make_shared<size_animator>(new_instance, _start, _end, _speed);
	}

	void size_animator::serialize(data::wfile* file)
	{
		THROW_ERROR("Unimplemented code error.");
	}

	void size_animator::deserialize(data::rfile* file, scene* scene)
	{
		THROW_ERROR("Unimplemented code error.");
	}

}