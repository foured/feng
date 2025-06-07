#pragma once

#include "../data_management/serializable.h"
//#include "../data_management/files.h"

namespace feng {

	class tag : public data::serializable  {
	public:
		virtual void serialize(data::wfile* file) = 0;
		virtual void deserialize(data::rfile* file, scene* scene) = 0;
	};

}
