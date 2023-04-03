#pragma once
#include <memory>
#include "blrnaBase.h"

namespace blrna
{
	class Attribute final : public Base
	{
	public:
		enum class AttrDomain : int {
			AUTO = -1,
			POINT = 0,
			EDGE = 1,
			FACE = 2,
			CORNER = 3,
			CURVE = 4,
			INSTANCE = 5,
		};

		enum class DataType : int {
			FLOAT = 10,
			INT32 = 11,
			STRING = 12,
			BYTE_COLOR = 17,
			INT8 = 45,
			COLOR = 47,
			FLOAT3 = 48,
			FLOAT2 = 49,
			BOOL = 50,
		};

		Attribute(py::object o) : Base(o) {}
		Attribute(PointerRNA r) : Base(r) {}

		ENUM_PROP(data_type)
		ENUM_PROP(domain)

		static bool Equals(PointerRNA r, AttrDomain domain, DataType data_type, const char* name = nullptr);
	};

	class IntAttribute final : public Base
	{
	public:
		IntAttribute(py::object o) : Base(o) {}
		IntAttribute(PointerRNA r) : Base(r) {}

		COLLECTION_PROP(data)
	};

	class BoolAttribute final : public Base
	{
	public:
		BoolAttribute(py::object o) : Base(o) {}
		BoolAttribute(PointerRNA r) : Base(r) {}

		COLLECTION_PROP(data)
	};

	class IColorAttribute
	{
	public:
		virtual void get_color(int n, float color[4]) const = 0;
	};

	class ColorAttribute final
	{
	public:
		ColorAttribute(py::object o);

		std::string get_name() const;
		EnumPropertyItem* get_domain_item() const;
		void get_color(int n, float color[4]) const;

	private:
		Attribute _attribute;
		std::unique_ptr<IColorAttribute> _colorAttribute;
	};
}
