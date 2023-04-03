#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class ID : public Base
	{
	public:
		ID(py::object o) : Base(o) {}
		ID(PointerRNA r) : Base(r) {}

		POINTER_PROP(original);
		POINTER_PROP(override_library);
		POINTER_PROP_ITEM_TYPE(override_library);
	};
}

