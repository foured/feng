#pragma once

#include <vector>
#include <map>
#include <functional>
#include <cassert>

#include "../logging/logging.h"

namespace feng {

	template<typename... Args>
	class event {
	public:
		class subscription {
		public:
			subscription() {}

			~subscription() {
				unsubscribe();
			}

			subscription(const subscription&) = delete;
			subscription& operator=(const subscription&) = delete;

			subscription(subscription&& other) noexcept
				: _id(other._id), _event(std::move(other._event)) {
				other._id = 0;
				other._event = nullptr;
			}

			subscription& operator=(subscription&& other) noexcept {
				if (this != &other) {
					unsubscribe();
					_id = other._id;
					_event = std::move(other._event);
					other._id = 0;
					other._event = nullptr;
				}
				return *this;
			}

		private:
			friend class event;

			void unsubscribe() {
				if (_event != nullptr && _id != 0) {
					_event->unsubscribe(_id);
					_event = nullptr;
					_id = 0;
				}
			}

			subscription(uint32_t id, event* e)
				: _id(id), _event(e) { }

			uint32_t _id = 0;
			event* _event = nullptr;
		};

		using func = std::function<void(Args...)>;

		[[nodiscard]] subscription subscribe(func f) {
			_observers.emplace(_current_id, f);
			_current_id++;
			return subscription(_current_id - 1, this);
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
		uint32_t _current_id = 1;
		std::map<uint32_t, func> _observers;

	};

}