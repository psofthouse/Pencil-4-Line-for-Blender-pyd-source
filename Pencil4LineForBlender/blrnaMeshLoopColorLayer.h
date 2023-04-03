#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class MeshLoopColorLayer final : public Base
	{
	public:
		MeshLoopColorLayer(py::object o) : Base(o) {}
		MeshLoopColorLayer(PointerRNA r) : Base(r) {}

		void SetData(void* ptr) { _PRNA.data = ptr; }

		COLLECTION_PROP(data);

	private:

	};
}
