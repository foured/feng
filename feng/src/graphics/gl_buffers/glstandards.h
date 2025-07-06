#pragma once

#include <map>
#include <typeindex>
#include "../../utilities/utilities.h"

#define TID typeid
#define TS sizeof

#define BASE_ALIGNMENT(T) feng::glstd::base_aligments[TID(T)]

#define IS_ALIGNMENTABLE(T) \
	const auto& t = TID(T);\
	FENG_ASSERT(feng::glstd::base_aligments.contains(t), "Can`t find 'base aligment' for type: ", t.name());

namespace feng::glstd {

	enum class buffer_std : uint8_t{
		std140 = 0,
		std430 = 1
	};

	static std::map<std::type_index, uint32_t> base_aligments {
		{ TID(int), 4 },
		{ TID(float), 4 },
		{ TID(glm::vec2), 8 },
		{ TID(glm::vec3), 16 },
		{ TID(glm::vec4), 16 },
		{ TID(glm::mat4), 16 }
	};

	struct buffer_element {
		uint32_t size = 0;
		uint32_t aligned_size = 0;
		bool is_empty = false;
	};

	struct buffer_structure {
	public:
		template<typename... Ts>
		void add_elements() {
			(add_element<Ts>(), ...);
		}

		template<typename T>
		void add_element() {
			IS_ALIGNMENTABLE(T);
			uint32_t alignment = BASE_ALIGNMENT(T);
			if (_array_mode) {
				alignment = utilities::round_to(alignment, 16);
			}
			_size = utilities::round_to(_size, alignment);
			elements.emplace_back(TS(T), alignment);
			_size += TS(T);
		}

		void add_struct(const buffer_structure& bs) {
			for(const auto& e : bs.elements){
				_size = utilities::round_to(_size, e.aligned_size);
				elements.emplace_back(e);
				_size += e.size;
			}
			uint32_t ssize = _size;
			_size = utilities::round_to(_size, 16);
			elements.emplace_back(_size - ssize, 0, true);
		}

		uint32_t size(bool round = true) const {
			if (round) {
				return utilities::round_to(_size, 16);
			}
			else {
				return _size;
			}
		}

		std::vector<buffer_element> elements;

		template<typename... Ts>
		static const buffer_structure make_buffer_structure() {
			buffer_structure buf;
			(buf.add_element<Ts>(), ...);
			return buf;
		}

		void start_array() {
			_array_mode = true;
		}

		void end_array() {
			_array_mode = false;
		}

	private:
		uint32_t _size = 0;
		bool _array_mode = false;

	};

}