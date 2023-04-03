#pragma once
#include "blrnaBase.h"

namespace blrna
{
	class Image final : public Base
	{
	public:
		Image(py::object image) : Base(image) {}
		~Image() {}

		int Width() const { int s[2]; get_size(s); return s[0]; }
		int Height() const { int s[2]; get_size(s); return s[1]; }

		FLOAT_ARRAY_PROP(pixels);
		INT_ARRAY_PROP(size);
		POINTER_PROP(colorspace_settings);
		BOOL_PROP(is_float)

	private:

	};

	class ColorManagedInputColorspaceSettings final : public Base
	{
	public:
		ColorManagedInputColorspaceSettings(py::object image) : Base(image) {}
		ColorManagedInputColorspaceSettings(PointerRNA r) : Base(r) {}
		~ColorManagedInputColorspaceSettings() {}

		ENUM_PROP(name)

	private:

	};
}
