#include "rigidbody.h"

#include "../instance.h"
#include "../../data_management/files.h"
#include "../scene.h"

#include "../../../physics/physics.h"
#include "../../../utilities/utilities.h"

namespace feng {

	rigidbody::rigidbody(instance* instance, float mass)
		: component(instance), _mass(mass) {

	}

	void rigidbody::start() {
		_acceleration += physics::gravity_vec;
	}

	void rigidbody::late_start() {

	}

	void rigidbody::update() {
		float dt = utilities::delta_time();

		_acceleration += _accumulated_force / _mass;
		_accumulated_force = glm::vec3(0.0f);

		glm::vec3 pos = get_pos();

		pos += _velocity * dt + _acceleration * (dt * dt) * 0.5f;
		_velocity += _acceleration * dt;

		set_pos(pos);
	}

	std::shared_ptr<component> rigidbody::copy(instance* new_instance) {
		THROW_ERROR("Unimplemented code error.");
		return std::make_shared<rigidbody>(new_instance, _mass);
	}

	void rigidbody::add_force(const glm::vec3& force) {
		_accumulated_force += force;
	}

	void rigidbody::on_collision(const advanced_collision_data& data) {
		_velocity = glm::vec3(0);
		_acceleration = glm::vec3(0);
	}

	void rigidbody::serialize(data::wfile* file)
	{
		THROW_ERROR("Unimplemented code error.");
	}

	void rigidbody::deserialize(data::rfile* file, scene* scene)
	{
		THROW_ERROR("Unimplemented code error.");
	}

	// PRIVATE----------------------------------------------------------------------------------------------------------

	glm::vec3 rigidbody::get_pos() {
		return _instance->transform.get_position();
	}

	void rigidbody::set_pos(const glm::vec3& pos) {
		_instance->transform.set_position(pos);
	}

}