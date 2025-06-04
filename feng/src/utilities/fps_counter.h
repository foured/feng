#pragma once

#include <iostream>
#include "utilities.h"

namespace feng {
	class fps_counter {
	public:

		uint64_t fps = 0;
		double msdt = 0;

		fps_counter(double step = 0.5)
			: _step(step) {}

		void update() {
			_time += utilities::delta_time();
			_frames_count++;
			if (_time >= _step) {
				fps = _frames_count / _time;
				msdt = 1000.0 / fps;
				_time = 0;
				_frames_count = 0;
			}
		}
	
	private:
		double _time = 0;
		double _step = 0.5;
		uint64_t _frames_count = 0;

	};
}