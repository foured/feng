#include "box_collider.h"

#include "../instance.h"
#include "../../data_management/files.h"
#include "../scene.h"

namespace feng {

	namespace {
		float p = 1.0f;
		std::vector<glm::vec3> cube_points{
			{  p,  p,  p  },
			{  p, -p,  p  },
			{  p,  p, -p  },
			{  p, -p, -p  },
			{ -p,  p,  p  },
			{ -p,  p, -p  },
			{  p, -p, -p  },
			{ -p, -p, -p  }
		};

		std::vector<glm::vec3> cube_normals{
			{ 1, 0, 0},
			{ 0, 1, 0},
			{ 0, 0, 1}
		};
	}

	// PUBLIC-----------------------------------------------------------------------------------------------------------

	//box_collider::box_collider(instance* instance, const glm::vec3& offset, const glm::vec3& size,
	//	const glm::mat3& rotation_mat)
	//	: component(instance), sat_collider_base(cube_point, cube_normals),
	//	_offset(offset), _size(size), _rotation_matrix(rotation_mat) {

	//}

	box_collider::box_collider(instance* instance, const glm::vec3& offset, const glm::vec3& size, 
		const glm::vec3& rotation)
		: component(instance), sat_collider_base(cube_points, cube_normals),
		  _offset(offset), _size(size), _rotation(rotation) {
	}

	void box_collider::start() {
		update_collider_data();
	}

	void box_collider::update() {

	}

	std::shared_ptr<component> box_collider::copy(instance* new_instance) {
		return std::make_shared<box_collider>(new_instance, _offset, _size, _rotation);
	}

	void box_collider::update_collider_data() {
		glm::vec3 pos = _instance->transform.get_position() + _offset;
		glm::vec3 size = _instance->transform.get_size() * _size;
		glm::vec3 rot_vec = _instance->transform.get_rotation() + _rotation;

		glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);
		glm::mat4 R = utilities::deg2mat4x4(rot_vec);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), size);

		glm::mat4 model = T * R * S;
		
		size_t len = cube_points.size();
		_points.clear();
		_points.reserve(len);
		for (size_t i = 0; i < len; i++) {
			_points.push_back(utilities::mul(model, cube_points[i]));
		}

		len = cube_normals.size();
		_normals.clear();
		_normals.reserve(len);
		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
		for (size_t i = 0; i < len; i++) {
			_normals.push_back(glm::normalize(utilities::mul(normal_matrix, cube_normals[i])));
		}
		_was_changed_after_update = false;
	}

	void box_collider::check_changes() {
		if (_instance->transform.changed_this_frame) {
			_was_changed_after_update = true;
		}
	}

	void box_collider::serialize(data::wfile* file)
	{
		THROW_ERROR("Unimplemented code error.");
	}

	void box_collider::deserialize(data::rfile* file, scene* scene)
	{
		THROW_ERROR("Unimplemented code error.");
	}

}