#pragma once

#include <string>
#include <random>

#include "singleton.hpp"
#include "../logic/containers/binary_tree.hpp"

namespace feng::util {

	class uuid : public singleton<uuid> {
	public:
		uuid() : _rd(), _gen(_rd()) { }

		uint64_t generate() {
			std::uniform_int_distribution<uint64_t> dist(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());
			uint64_t v = dist(_gen);
			_ids.insert(v);
			return v;
		}

	private:
		containers::binary_tree<uint64_t> _ids;

		std::random_device _rd;
		std::mt19937_64 _gen;

	};

}
