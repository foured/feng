#pragma once

#include <vector>
#include <memory>

#include "../../fng.h"
#include "transform.h"
#include "component.h"
#include "../../logging/logging.h"

#define INST_FLAG_STATIC				0b0000000000000001
#define INST_FLAG_CAST_SHADOWS			0b0000000000000010
#define INST_FLAG_CAST_RCV_SHADOWS		0b0000000000000100

namespace feng {

	class instance_flags {
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
		uint16_t _flags;
	};

	class instance {
	public:
		transform transform;

		bool is_active = true;
		instance_flags flags;

		void start();
		void update();

		template<typename T, typename... Args>
		typename std::enable_if_t<std::is_base_of_v<component, T> && std::is_constructible_v<T, instance*, Args...>, std::shared_ptr<T>>
		add_component(Args... args) {
			std::shared_ptr<T> spc = std::make_shared<T>(this, std::forward<Args>(args)...);
			_components.emplace_back(spc);
			return spc;
		}

	private:
		std::vector<std::shared_ptr<component>> _components;

	};

}