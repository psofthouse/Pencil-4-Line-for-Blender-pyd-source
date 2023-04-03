#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class MEdge final : public Base
	{
	public:
		MEdge(py::object o) : Base(o) {}
		MEdge(PointerRNA r) : Base(r) {}

		void SetMEdge(void* ptr) { _PRNA.data = ptr; }

		BOOL_PROP(use_freestyle_mark);

	private:

	};
}
