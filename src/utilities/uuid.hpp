#pragma once

#include <string>
#include <random>
	
#include "singleton.hpp"
#include "../algorithms/binary_tree.hpp"
#include "../logging/logging.h"

#include "../fng.h"

namespace feng::util {

	class uuid : public singleton<uuid> {
	public:
		uuid() : _rd(), _gen(_rd()), _dist(std::numeric_limits<uuid_type>::min(), std::numeric_limits<uuid_type>::max()){ }

		uuid_type generate() {
			uuid_type v = _dist(_gen);
			_ids.insert(v);
			return v;
		}

		void insert(uuid_type id) {
			if (_ids.contains(id)) {
				LOG_WARNING("ID: [" + std::to_string(id) + "] is not unique.");
				return;
			}
			_ids.insert(id);
		}

	private:
		friend class uuid_owner;

		binary_tree<uuid_type> _ids;

		std::random_device _rd;
		std::mt19937_64 _gen;
		std::uniform_int_distribution<uuid_type> _dist;

	};

	class uuid_owner {
	public:
		uuid_owner() : _uuid(uuid::get_instance()->generate()) { }
		uuid_owner(uuid_type id) : _uuid(id) { }

		std::string get_uuid_string() {
			return "uuid: [" + std::to_string(_uuid) + "]";
		}

		uuid_type get_uuid() const {
			return _uuid;
		}

		void reset(uuid_type new_uuid) {
			uuid::get_instance()->_ids.remove(_uuid);
			_uuid = new_uuid;
			uuid::get_instance()->insert(_uuid);
		}

	private:
		uuid_type _uuid;

	};

}