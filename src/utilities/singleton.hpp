#pragma once

namespace feng::util {

	template<typename T>
	class singleton {
	public:
		singleton(const singleton& s) = delete;
		singleton& operator=(const singleton&) = delete;

		static T* get_instance() {
			static T instance;
			return &instance;
		}

	protected:
		singleton() {}

	};

}