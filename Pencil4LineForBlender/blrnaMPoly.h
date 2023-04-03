#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class MPoly final : public Base
	{
	public:
		MPoly(py::object o) : Base(o) {}
		MPoly(PointerRNA r) : Base(r) {}

		void SetMPoly(const void* ptr) { _PRNA.data = const_cast<void*>(ptr); }

		INT_PROP(material_index);

	private:

	};
}
