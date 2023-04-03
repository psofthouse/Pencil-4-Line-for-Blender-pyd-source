#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class Material : public Base
	{
	public:
		Material(py::object o) : Base(o) {}
		Material(PointerRNA r) : Base(r) {}

		void SetData(void* ptr) { _PRNA.data = ptr; }

		BOOL_PROP(use_backface_culling);
	};
}

