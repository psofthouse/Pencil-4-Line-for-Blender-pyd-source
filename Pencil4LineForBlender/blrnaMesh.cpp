#include "blrnaMesh.h"

namespace blrna
{
	DEF_COLLECTION_PROP_RNA(Mesh, vertices);
	DEF_COLLECTION_PROP_RNA(Mesh, edges);
	DEF_COLLECTION_PROP_RNA(Mesh, polygons);
	DEF_COLLECTION_PROP_RNA(Mesh, loops);
	DEF_COLLECTION_PROP_RNA(Mesh, loop_triangles);
	DEF_COLLECTION_PROP_RNA(Mesh, loop_triangle_polygons);

	DEF_COLLECTION_PROP_RNA(Mesh, materials);

	DEF_COLLECTION_PROP_RNA(Mesh, uv_layers);
	DEF_COLLECTION_PROP_RNA(Mesh, vertex_colors);
	DEF_COLLECTION_PROP_RNA(Mesh, attributes);

	DEF_BOOL_PROP_RNA(Mesh, use_auto_smooth);
	DEF_BOOL_PROP_RNA(Mesh, has_custom_normals);
	DEF_FLOAT_PROP_RNA(Mesh, auto_smooth_angle);

	DEF_FUNC_RNA(Mesh, calc_loop_triangles);
	DEF_FUNC_RNA(Mesh, calc_normals_split);
}
