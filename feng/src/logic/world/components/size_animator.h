#pragma once

#include <glm/glm.hpp>
#include "../component.h"

namespace feng {

	class size_animator : public component {
	public:
		size_animator(instance* instance, glm::vec3 start_size, glm::vec3 end_size, float speed);
		
		void start();
		void update();
		std::shared_ptr<component> copy(instance* new_instance);

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		glm::vec3 _start;
		glm::vec3 _end;
		glm::vec3 _dir;
		float _speed;
		float _mtime = 0, _ctime = 0;
		bool _fets = false;

	};

}