#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class IDOverrideLibrary : public Base
	{
	public:
		IDOverrideLibrary(py::object o) : Base(o) {}
		IDOverrideLibrary(PointerRNA r) : Base(r) {}

		POINTER_PROP(reference);
	};
}

