#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class Camera final : public Base
	{
	public:
		Camera(py::object o) : Base(o) {}
		Camera(PointerRNA r) : Base(r) {}
		~Camera() {}

		FLOAT_PROP(clip_start);
		FLOAT_PROP(clip_end);

	private:

	};
}
