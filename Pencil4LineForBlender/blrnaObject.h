#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class Object final : public Base
	{
	public:
		Object(py::object o) : Base(o) {}
		~Object() {}

		POINTER_PROP(data);
		FLOAT_ARRAY_PROP(matrix_world);

	private:

	};
}
