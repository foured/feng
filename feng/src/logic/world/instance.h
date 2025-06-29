#pragma once

#include <vector>
#include <memory>

#include "../../fng.h"
#include "transform.h"
#include "component.h"
#include "../../logging/logging.h"
#include "../../utilities/uuid.hpp"
#include "../data_management/serializable.h"
#include "tag.h"

#define INST_FLAG_STATIC			0b0000000000000001
#define INST_FLAG_CAST_SHADOWS		0b0000000000000010
#define INST_FLAG_RCV_SHADOWS		0b0000000000000100

namespace feng {

	class scene;

	class instance : public util::uuid_owner, public data::serializable {
	public:
		class flags {
		public:
			void set(inst_flag_type pos, bool value) {
				if (value)
					_flags |= pos;
				else
					_flags &= ~pos;
			}

			bool get(inst_flag_type pos) const {
				return (_flags & pos) != 0;
			}

		private:
			friend class instance;

			inst_flag_type _flags = (INST_FLAG_STATIC | INST_FLAG_CAST_SHADOWS | INST_FLAG_RCV_SHADOWS);
		};

		flags flags;
		bool is_active = true;
		transform transform;

		void start();
		void update();

		//==================
		// COMPONENTS
		//==================

		template<typename T, typename... Args>
		typename std::enable_if_t<std::is_base_of_v<component, T> && std::is_constructible_v<T, instance*, Args...>, std::shared_ptr<T>>
		add_component(Args... args) {
			std::shared_ptr<T> spc = std::make_shared<T>(this, std::forward<Args>(args)...);
			_components.push_back(spc);
			return spc;
		}

		template<typename T>
		typename std::enable_if_t<std::is_base_of_v<component, T>, std::shared_ptr<T>>
		try_get_component() const {
			for (const auto& component : _components) {
				if (auto t = std::dynamic_pointer_cast<T>(component)) {
					return t;
				}
			}
			return std::shared_ptr<T>();
		}

		template<typename T>
		std::shared_ptr<T> try_find_component_of_type() const {
			for (const auto& component : _components) {
				if (auto t = std::dynamic_pointer_cast<T>(component)) {
					return t;
				}
			}
			return std::shared_ptr<T>();
		}

		template<typename T, typename wrapper = std::shared_ptr<T>>
		std::vector<wrapper> try_find_components_of_type() const {
			std::vector<wrapper> ret;
			for (const auto& component : _components) {
				if (auto t = std::dynamic_pointer_cast<T>(component)) {
					ret.push_back(t);
				}
			}
			return ret;
		}

		//==================
		// TAGS
		//==================
		template<typename T, typename... Args>
		typename std::enable_if_t<std::is_base_of_v<tag, T> && std::is_constructible_v<T, Args...>, std::shared_ptr<T>>
		add_tag(Args... args) {
			std::shared_ptr<T> t = std::make_shared<T>(std::forward<Args>(args)...);
			_tags.push_back(t);
			return t;
		}

		template<typename T, typename... Args>
		typename std::enable_if_t<std::is_base_of_v<tag, T>, std::shared_ptr<T>>
		try_get_tag() const {
			for (const auto& tag : _tags) {
				if (auto t = std::dynamic_pointer_cast<T>(tag)) {
					return t;
				}
			}
			return std::shared_ptr<T>();
		}


		std::shared_ptr<instance> copy() const;

		void serialize(data::wfile* file) override;
		void deserialize(data::rfile* file, feng::scene* scene) override;

	private:
		std::vector<std::shared_ptr<component>> _components;
		std::vector<std::shared_ptr<tag>> _tags;

	};

}