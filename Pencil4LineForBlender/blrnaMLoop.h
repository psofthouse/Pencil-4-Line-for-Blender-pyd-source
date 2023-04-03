#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class MLoop final : public Base
	{
	public:
		MLoop(py::object o) : Base(o) {}
		MLoop(PointerRNA r) : Base(r) {}

		void SetMLoop(const void* ptr) { _PRNA.data = const_cast<void*>(ptr); }

		FLOAT_ARRAY_PROP(normal);

	private:

	};
}
