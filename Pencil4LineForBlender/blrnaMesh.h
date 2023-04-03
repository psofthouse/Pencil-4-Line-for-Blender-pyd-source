#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class Mesh : public Base
	{
	public:
		Mesh(py::object o) : Base(o) {}
		Mesh(PointerRNA r) : Base(r) {}

		COLLECTION_PROP(vertices);
		COLLECTION_PROP(edges);
		COLLECTION_PROP_ITEM_TYPE(edges);
		COLLECTION_PROP(polygons);
		COLLECTION_PROP_ITEM_TYPE(polygons);
		COLLECTION_PROP(loops);
		COLLECTION_PROP(loop_triangles);
		COLLECTION_PROP_ITEM_TYPE(loops);
		COLLECTION_PROP_ITEM_TYPE(loop_triangles);

		COLLECTION_PROP(materials);
		COLLECTION_PROP_ITEM_TYPE(materials);

		COLLECTION_PROP(uv_layers);
		COLLECTION_PROP_LOOKUP_INT(uv_layers);

		COLLECTION_PROP(vertex_colors);
		COLLECTION_PROP_LOOKUP_INT(vertex_colors);

		COLLECTION_PROP(attributes);

		BOOL_PROP(use_auto_smooth);
		BOOL_PROP(has_custom_normals);
		FLOAT_PROP(auto_smooth_angle);

		ACTION(calc_loop_triangles);
		ACTION(calc_normals_split);
	};
}

