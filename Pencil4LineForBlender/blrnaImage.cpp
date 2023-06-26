#include "blrnaImage.h"

namespace blrna
{
	DEF_FLOAT_PROP_RNA(Image, pixels);
	DEF_INT_PROP_RNA(Image, size);
	DEF_POINTER_PROP_RNA(Image, colorspace_settings);
	DEF_BOOL_PROP_RNA(Image, is_float);
	DEF_FUNC_RNA(Image, update);

	DEF_ENUM_PROP_RNA(ColorManagedInputColorspaceSettings, name);
}
