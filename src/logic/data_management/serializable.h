#pragma once

namespace feng{
	class scene;
}

namespace feng::data {

	class rfile;
	class wfile;

	class serializable {
	public:
		virtual ~serializable() = default;

		virtual void serialize(wfile* file) = 0;
		virtual void deserialize(rfile* file, scene* scene) = 0;
	};

}