#pragma once

#include "../component.h"
#include <glm/glm.hpp>

namespace feng {

	class line_animator : public component {
	public:
		line_animator(instance* instance, glm::vec3 start, glm::vec3 end, float speed);
		
		void start() override;
		void late_start() override;
		void update() override;
		std::shared_ptr<component> copy(instance* new_instance);

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, scene* scene) override;

	private:
		friend class instance;

		glm::vec3 _start; 
		glm::vec3 _end;
		glm::vec3 _dir;
		float _speed;
		float _mtime = 0, _ctime = 0;
		bool _fets = false;
	};

}