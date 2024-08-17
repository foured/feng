#pragma once

#include <vector>
#include <map>
#include <functional>

namespace feng {

	template<typename... Args>
	class event {
	public:
		using func = std::function<void(Args...)>;

		int subscribe(func f) {
			_observers.emplace(_current_id, f);
			_current_id++;
			return _current_id - 1;
		}

		void unsubscribe(uint32_t id) {
			_observers.erase(id);
		}

		void invoke(Args... args) {
			for (const auto& p : _observers) {
				p.second(args...);
			}
		}

	private:
		uint32_t _current_id = 0;
		std::map<uint32_t, func> _observers;

	};

}