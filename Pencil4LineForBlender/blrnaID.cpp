#include "blrnaID.h"

#pragma warning( push )
#pragma warning( disable : 4200 ) // zero length array
#include "intern/rna_internal_types.h"
#pragma warning( pop ) 

namespace blrna
{
	static PointerRNA& GetRootIdType(PointerRNA& prna) { while (prna.type->base) { prna.type = prna.type->base; }  return prna;  }

	void* ORIGINAL_PTR(PointerRNA prna) { return ID(GetRootIdType(prna)).get_original().data; }
	void* OVERRIDE_LIBRARY_PTR(PointerRNA prna) { return ID(GetRootIdType(prna)).get_override_library().data; }
	StructRNA* OVERRIDE_LIBRARY_ITEM_TYPE(PointerRNA prna) { return ID(GetRootIdType(prna)).override_library_item_type(); }


	DEF_POINTER_PROP_RNA(ID, original);
	DEF_POINTER_PROP_RNA(ID, override_library);
}
