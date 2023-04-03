#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class MeshUVLoopLayer final : public Base
	{
	public:
		MeshUVLoopLayer(py::object o) : Base(o) {}
		MeshUVLoopLayer(PointerRNA r) : Base(r) {}

		void SetData(void* ptr) { _PRNA.data = ptr; }

		COLLECTION_PROP(data);
		COLLECTION_PROP(uv);
		HAS_PROP(uv);

	private:

	};
}
