#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class MLoopTri final : public Base
	{
	public:
		MLoopTri(py::object o) : Base(o) {}
		MLoopTri(PointerRNA r) : Base(r) {}

		FLOAT_ARRAY_PROP(split_normals);

	private:

	};
}
