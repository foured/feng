#pragma once

#include "gl_buffer.hpp"

namespace feng {

	class arraybuffer : public gl_buffer{
	public:
		arraybuffer(bool autogen = false) 
			: gl_buffer(GL_ARRAY_BUFFER, autogen) { }
	};

}