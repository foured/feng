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
		find_independent(_instance);
		update_collider_data();
	}

	void box_collider::late_start() {

	}

	void box_collider::update() {

	}

	std::shared_ptr<component> box_collider::copy(instance* new_instance) {
		return std::make_shared<box_collider>(new_instance, _offset, _size, _rotation);
	}

	void box_collider::update_collider_data() {
		//glm::vec3 pos = _instance->transform.get_position() + _offset;
		//glm::vec3 size = _instance->transform.get_size() * _size;
		//glm::vec3 rot_vec = _instance->transform.get_rotation() + _rotation;

		glm::vec3 pos = get_position();;
		glm::vec3 size = get_size();
		glm::vec3 rot_vec = get_rotation();

		glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);
		glm::mat4 R = utilities::deg2mat4x4(rot_vec);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), size);

		glm::mat4 model = T * R * S;
		
		aabb bounds = get_base().limit_size();
		std::array<glm::vec3, 8> corners = bounds.corners();

		size_t len = corners.size();
		_points.clear();
		_points.reserve(len);
		for (size_t i = 0; i < len; i++) {
			_points.push_back(utilities::mul(model, corners[i]));
		}

		len = cube_normals.size();
		_normals.clear();
		_normals.reserve(len);
		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(model)));
		for (size_t i = 0; i < len; i++) {
			_normals.push_back(glm::normalize(utilities::mul(normal_matrix, cube_normals[i])));
		}
		calculate_center();
		_was_changed_after_update = false;
	}

	void box_collider::check_changes() {
		if (_instance->transform.changed_this_frame) {
			_was_changed_after_update = true;
		}
	}

	bool box_collider::is_static() const {
		return _instance->flags.get(INST_FLAG_STATIC);
	}

	void box_collider::add_position(const glm::vec3& offset) {
		_instance->transform.add_position(offset);
	}

	void box_collider::on_collision(const advanced_collision_data& data) {
		//LOG_INFO(data.collision_data->contact.type_to_int());
		//const polygon* pol = data.collision_data->contact.get_pointer<polygon>();
		//for (const glm::vec3& point : pol->points) {
		//	LOG_INFO(point);
		//}
		//const glm::vec3* point = data.collision_data->contact.get_pointer<glm::vec3>();
		//LOG_INFO(*point);
	}	

	glm::vec3 box_collider::get_position() const {
		return _independent.lock()->get_position() + _offset;
	}

	glm::vec3 box_collider::get_size() const {
		return _independent.lock()->get_size() * _size;
	}

	glm::vec3 box_collider::get_rotation() const {
		return _independent.lock()->get_rotation() + _rotation;
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