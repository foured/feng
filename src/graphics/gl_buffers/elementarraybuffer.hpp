#pragma once

#include "gl_buffer.hpp"

namespace feng {
	class elementarraybuffer : public gl_buffer {
	public:
		elementarraybuffer(bool autogen = false) 
			: gl_buffer(GL_ELEMENT_ARRAY_BUFFER, autogen) { }
	};

}