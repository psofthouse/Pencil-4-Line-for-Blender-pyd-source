#pragma once
#include "pybind11/pybind11.h"
namespace py = pybind11;

#pragma warning( push )
#pragma warning( disable : 4200 ) // zero length array
#include "RNA_define.h"
#include "intern/rna_internal_types.h"
#include "intern/bpy_rna.h"
#pragma warning( pop ) 

#if defined(_MSC_VER)
#define msPacked 
#else
#define msPacked __attribute__((packed))
#endif

namespace blrna
{
#define PROP_RNA(P) g_prop_##P
#define ENUM_ITEMS(P) g_items_##P
#define DEF_PROP_RNA(cls, type, P) type * cls ::PROP_RNA(P) = nullptr
#define DEF_FLOAT_PROP_RNA(cls, P) DEF_PROP_RNA(cls, FloatPropertyRNA, P)
#define DEF_INT_PROP_RNA(cls, P) DEF_PROP_RNA(cls, IntPropertyRNA, P)
#define DEF_BOOL_PROP_RNA(cls, P) DEF_PROP_RNA(cls, BoolPropertyRNA, P)
#define DEF_POINTER_PROP_RNA(cls, P) DEF_PROP_RNA(cls, PointerPropertyRNA, P)
#define DEF_COLLECTION_PROP_RNA(cls, P) DEF_PROP_RNA(cls, CollectionPropertyRNA, P)
#define DEF_ENUM_PROP_RNA(cls, P) DEF_PROP_RNA(cls, EnumPropertyRNA, P); EnumPropertyItem* cls ::ENUM_ITEMS(P) = nullptr

#define VALUE_PROP(T, TR, P)\
	static TR* PROP_RNA(P);\
	void set_##P (T v) { FetchProp(PROP_RNA(P), #P); PROP_RNA(P)->set(&_PRNA, v); }\
	T get_##P () const { FetchProp(PROP_RNA(P), #P); auto rna = _PRNA; return PROP_RNA(P)->get(&rna); }\

#define FLOAT_PROP(P) VALUE_PROP(float, FloatPropertyRNA, P)
#define INT_PROP(P) VALUE_PROP(int, IntPropertyRNA, P)
#define BOOL_PROP(P) VALUE_PROP(bool, BoolPropertyRNA, P)
#define ENUM_PROP(P) VALUE_PROP(int, EnumPropertyRNA, P)\
	static EnumPropertyItem* ENUM_ITEMS(P);\
	EnumPropertyItem* get_##P##_items() const { FetchProp(PROP_RNA(P), #P); FetchItems(ENUM_ITEMS(P), PROP_RNA(P)); return ENUM_ITEMS(P); }\
	EnumPropertyItem* get_##P##_item() const { return EnumItem(get_##P##_items(), get_##P ()); }\


#define ARRAY_PROP(T, TR, P)\
	static TR* PROP_RNA(P);\
	void set_##P (const T* buff) { FetchProp(PROP_RNA(P), #P); PROP_RNA(P)->setarray(&_PRNA, buff); }\
	void get_##P (T* buff) const { FetchProp(PROP_RNA(P), #P); auto rna = _PRNA; PROP_RNA(P)->getarray(&rna, buff); }\

#define FLOAT_ARRAY_PROP(P) ARRAY_PROP(float, FloatPropertyRNA, P)
#define INT_ARRAY_PROP(P) ARRAY_PROP(int, IntPropertyRNA, P)


#define POINTER_PROP(P)\
	static PointerPropertyRNA* PROP_RNA(P);\
	void set_##P (PointerRNA v) { FetchProp(PROP_RNA(P), #P); PROP_RNA(P)->set(&_PRNA, v, nullptr); }\
	PointerRNA get_##P () const { FetchProp(PROP_RNA(P), #P); auto rna = _PRNA; return PROP_RNA(P)->get(&rna); }\

#define	POINTER_PROP_ITEM_TYPE(P)\
	StructRNA* P##_item_type() const { FetchProp(PROP_RNA(P), #P); return PROP_RNA(P)->type; }\


#define COLLECTION_PROP(P)\
	static CollectionPropertyRNA* PROP_RNA(P);\
	int get_##P##_length() const { FetchProp(PROP_RNA(P), #P); auto rna = _PRNA; return PROP_RNA(P)->length(&rna); }\
	template<class T> T get_##P () const { FetchProp(PROP_RNA(P), #P); auto rna = _PRNA; CollectionPropertyIterator itr; PROP_RNA(P)->begin(&itr, &rna); return (T)itr.internal.array.ptr; } \
	template<class F> void foreach_##P (F func) const { FetchProp(PROP_RNA(P), #P); foreach(PROP_RNA(P), func); } \
	template<class T> size_t get_##P##_data_step() const { FetchProp(PROP_RNA(P), #P); return data_step<T>(PROP_RNA(P)); } \

#define	COLLECTION_PROP_ITEM_TYPE(P)\
	StructRNA* P##_item_type() const { FetchProp(PROP_RNA(P), #P); return PROP_RNA(P)->item_type; }\

#define	COLLECTION_PROP_LOOKUP_INT(P)\
	PointerRNA P##_lookup_int(int i) const { FetchProp(PROP_RNA(P), #P); auto rna = _PRNA; PointerRNA ret; PROP_RNA(P)->lookupint(&rna, i, &ret); return ret; } \


#define HAS_PROP(P)\
	bool has_##P##_prop() const { static bool ret = FetchProp(PROP_RNA(P), #P); return ret; }


#define FUNC_RNA(F) g_func_##F
#define DEF_FUNC_RNA(cls, F) FunctionRNA* cls ::FUNC_RNA(F) = nullptr

#define ACTION(F)\
	static FunctionRNA* FUNC_RNA(F);\
	void F () { FetchFunc(FUNC_RNA(F), #F); return Action(FUNC_RNA(F)); }

//#define FUNC3(F, T0, T1, T2)\
//	static FunctionRNA* FUNC_RNA(F);\
//	void F##(T0& arg0, T1& arg1, T2& arg2) { FetchFunc(FUNC_RNA(F), #F); return Func(FUNC_RNA(F), arg0, arg1, arg2); }


#define forListBase(type, v, listbase)  for(auto v = (type)listbase.first; v; v = (type)((Link*)v)->next)
#define forListBaseC(type, v, listbase, additional_condition)  for(auto v = (type)listbase.first; v && additional_condition ; v = (type)((Link*)v)->next)

	const PointerRNA NULL_PointerRNA(nullptr, nullptr, nullptr);
	inline PointerRNA ConvertToPointerRNA(py::object obj) { return (obj && !obj.is_none()) ? ((BPy_StructRNA*)obj.ptr())->ptr : NULL_PointerRNA; }
	inline void* ConvertToRNAData(py::object obj) { return ConvertToPointerRNA(obj).data; }

	extern void* ORIGINAL_PTR(PointerRNA id);
	extern void* OVERRIDE_LIBRARY_PTR(PointerRNA id);
	extern StructRNA* OVERRIDE_LIBRARY_ITEM_TYPE(PointerRNA id);

	class Base
	{
	public:
		Base(py::object obj) :_PRNA(ConvertToPointerRNA(obj))
		{}
		Base(PointerRNA prna) :_PRNA(prna)
		{}

		void* data() const { return _PRNA.data; }
		explicit operator bool() const { return data(); }

		void* original_data() const				{ return ORIGINAL_PTR(_PRNA); }
		template<class T> T original() const	{ return T(PointerRNA{ _PRNA.owner_id, _PRNA.type, original_data()}); }

		void* override_library_data() const				{ return OVERRIDE_LIBRARY_PTR(_PRNA); }
		template<class T> T override_library() const	{ return T(PointerRNA{ _PRNA.owner_id, _PRNA.type, override_library_data() }); }
		StructRNA* override_library_item_type() const	{ return OVERRIDE_LIBRARY_ITEM_TYPE(_PRNA); }

		std::string get_struct_name() const
		{
			auto prop = reinterpret_cast<StringPropertyRNA*>(_PRNA.type->nameproperty);
			if (prop)
			{
				auto rna = _PRNA;
				if (prop->get)
				{
					auto buff = std::vector<char>(prop->length(&rna) + 1, 0);
					prop->get(&rna, buff.data());
					return std::string(buff.data());
				}
				else if (prop->get_ex)
				{
					auto buff = std::vector<char>(prop->length_ex(&rna, _PRNA.type->nameproperty) + 1, 0);
					prop->get_ex(&rna, _PRNA.type->nameproperty, buff.data());
					return std::string(buff.data());
				}
				else
				{
					return std::string(prop->defaultvalue);
				}
			}
			return std::string();
		}

	protected:
		PointerRNA _PRNA;

		template<class T> bool FetchProp(T*& out, const char* name) const
		{
			forListBaseC(PropertyRNA*, prop, _PRNA.type->cont.properties, out == nullptr)
			{
				out = (strcmp(prop->identifier, name) == 0) ? (T*)prop : out;
			}
			return out;
		}

		void FetchFunc(FunctionRNA*& out, const char* name) const
		{
			forListBaseC(FunctionRNA*, func, _PRNA.type->functions, out == nullptr)
			{
				out = (strcmp(func->identifier, name) == 0) ? func : out;
			}
		}

		void FetchItems(EnumPropertyItem*& out, EnumPropertyRNA* enumProp) const
		{
			if (!out)
			{
				bool free;
				auto prna = _PRNA;
				out = const_cast<EnumPropertyItem*>(enumProp->item_fn(nullptr, &prna, nullptr, &free));
			}
		}

		static EnumPropertyItem* EnumItem(EnumPropertyItem* items, int value)
		{
			for (; items && items->identifier; items++)
			{
				if (items->value == value)
				{
					return items;
				}
			}
			return nullptr;
		}

		void Action(FunctionRNA* f)
		{
			f->call(nullptr, nullptr, &_PRNA, nullptr);
		}

		template<class F> void foreach(CollectionPropertyRNA* propRNA, F func) const
		{
			auto rna = _PRNA;
			int len = propRNA->length(&rna);
			if (len > 0)
			{
				CollectionPropertyIterator itr;
				propRNA->begin(&itr, &rna);
				func(itr);

				for (int i = 1; i < len; i++)
				{
					propRNA->next(&itr);
					func(itr);
				}
			}
		}

		template<class T> size_t data_step(CollectionPropertyRNA* propRNA) const
		{
			auto rna = _PRNA;
			int len = propRNA->length(&rna);
			if (len <= 1)
			{
				return sizeof(T);
			}

			CollectionPropertyIterator itr;
			propRNA->begin(&itr, &rna);
			auto d0 = itr.internal.array.ptr;

			propRNA->next(&itr);
			auto d1 = itr.internal.array.ptr;

			return d1 - d0;
		}


		//template <class T0, class T1, class T2> void Func(FunctionRNA* f, T0& arg0, T1& arg1, T2& arg2)
		//{
		//	ParameterList params;
		//	char data[sizeof(T0) + sizeof(T1) + sizeof(T2)];
		//	params.data = data;

		//	f->call(nullptr, nullptr, &_PRNA, &params);
		//}


	};
}

