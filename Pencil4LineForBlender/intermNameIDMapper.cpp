#include "intermNameIDMapper.h"

namespace interm
{
	int NameIDMapper::GetID(const char* name)
	{
		return _map.emplace(name, (int)_map.size() + 1).first->second;
	}
}