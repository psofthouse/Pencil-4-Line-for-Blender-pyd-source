#include "blrnaAttribute.h"

namespace blrna
{
	class ByteColorAttribute final : public Base
	{
	public:
		ByteColorAttribute(py::object o) : Base(o) {}
		ByteColorAttribute(PointerRNA r) : Base(r) {}

		COLLECTION_PROP(data)
			COLLECTION_PROP_ITEM_TYPE(data)
	};

	class ByteColorAttributeValue final : public Base
	{
	public:
		ByteColorAttributeValue(py::object o) : Base(o) {}
		ByteColorAttributeValue(PointerRNA r) : Base(r) {}

		FLOAT_ARRAY_PROP(color);
	};

	class FloatColorAttribute final : public Base
	{
	public:
		FloatColorAttribute(py::object o) : Base(o) {}
		FloatColorAttribute(PointerRNA r) : Base(r) {}

		COLLECTION_PROP(data)
		COLLECTION_PROP_ITEM_TYPE(data)
	};

	class FloatColorAttributeValue final : public Base
	{
	public:
		FloatColorAttributeValue(py::object o) : Base(o) {}
		FloatColorAttributeValue(PointerRNA r) : Base(r) {}

		FLOAT_ARRAY_PROP(color);
	};


	DEF_ENUM_PROP_RNA(Attribute, data_type);
	DEF_ENUM_PROP_RNA(Attribute, domain);

	DEF_COLLECTION_PROP_RNA(IntAttribute, data);

	DEF_COLLECTION_PROP_RNA(BoolAttribute, data);

	DEF_COLLECTION_PROP_RNA(ByteColorAttribute, data);
	DEF_FLOAT_PROP_RNA(ByteColorAttributeValue, color);

	DEF_COLLECTION_PROP_RNA(FloatColorAttribute, data);
	DEF_FLOAT_PROP_RNA(FloatColorAttributeValue, color);


	bool Attribute::Equals(PointerRNA r, AttrDomain domain, DataType data_type, const char* name)
	{
		while (r.type && r.data)
		{
			if (strcmp(r.type->identifier, "Attribute") == 0)
			{
				auto attr = Attribute(r);
				if (static_cast<int>(data_type) == attr.get_data_type() && static_cast<int>(domain) == attr.get_domain())
				{
					if (!name)
					{
						return true;
					}
					return strcmp(attr.get_struct_name().data(), name) == 0;
				}
				return false;
			}
			r = PointerRNA( r.owner_id, r.type->base, r.data );
		}

		return false;
	}


	class INullColorAttribute : public IColorAttribute
	{
	public:
		void get_color(int n, float color[4]) const override
		{
			color[0] = 0;
			color[1] = 0;
			color[2] = 0;
			color[3] = 0;
		}
	};


	template<class T, class TV> class RnaColorAttribute : public IColorAttribute
	{
	public:
		RnaColorAttribute(PointerRNA prna):_attr(prna)
		{
			_pData = _attr.template get_data<char*>();
			_dataStep = _attr.template get_data_data_step<char>();
		}

		void get_color(int n, float color[4]) const override
		{
			static StructRNA* g_dataItemType = _attr.data_item_type();
			TV work(PointerRNA{ nullptr, g_dataItemType, _pData + _dataStep * n });
			work.get_color(color);
		}

	private:
		T _attr;
		size_t _dataStep;
		char* _pData;
	};

	ColorAttribute::ColorAttribute(py::object o):_attribute(NULL_PointerRNA)
	{
		PointerRNA prna = ConvertToPointerRNA(o);
		_attribute = Attribute(PointerRNA{ prna.owner_id, prna.type->base, prna.data });

		switch (_attribute.get_data_type())
		{
		case 17: // ByteColorAttribute
			_colorAttribute = std::make_unique<RnaColorAttribute<ByteColorAttribute, ByteColorAttributeValue>>(prna);
			break;
		case 47: // FloatColorAttribute
			_colorAttribute = std::make_unique<RnaColorAttribute<FloatColorAttribute, FloatColorAttributeValue>>(prna);
			break;
		default:
			_colorAttribute = std::make_unique<INullColorAttribute>();
			break;
		}
	}

	std::string ColorAttribute::get_name() const
	{
		return _attribute.get_struct_name();
	}

	EnumPropertyItem* ColorAttribute::get_domain_item() const
	{
		return _attribute.get_domain_item();
	}

	void ColorAttribute::get_color(int n, float color[4]) const
	{
		_colorAttribute->get_color(n, color);
	}
}
