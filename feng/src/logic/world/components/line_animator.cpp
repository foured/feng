#include "line_animator.h"

#include "../instance.h"
#include "../../../utilities/utilities.h"

namespace feng {

	line_animator::line_animator(instance* instance, glm::vec3 start, glm::vec3 end, float speed)
		: _start(start), _end(end), _speed(speed), _dir(glm::normalize(end - start)), component(instance) {
		_mtime = glm::length(_end - _start) / speed;
	}

	void line_animator::start() {
		_instance->transform.set_position(_start);
	}

	void line_animator::update() {
		float k = _fets ? -1 : 1;
		glm::vec3 d = _dir * _speed * (float)utilities::delta_time() * k;
		_instance->transform.set_position(_instance->transform.get_position() + d);
		_ctime += utilities::delta_time();
		if (_ctime >= _mtime) {
			_ctime = 0;
			_fets = !_fets;
		}
	}

	std::shared_ptr<component> line_animator::copy(instance* new_instance) {
		return std::make_shared<line_animator>(new_instance, _start, _end, _speed);
	}

}