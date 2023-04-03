#include "intermDataHash.h"
#include "meow_hash\meow_hash_x64_aesni.h"

namespace interm
{
	void DataHash::Record(size_t offset, size_t bytes, const void* pData)
	{
		_informations[offset] = Info{ bytes , MeowU64From(MeowHash(MeowDefaultSeed, bytes, const_cast<void*>(pData)), 0) };
	}

	bool DataHash::operator==(const DataHash& other) const
	{
		if (_informations.size() != other._informations.size())
		{
			return false;
		}

		for (const auto& item : _informations)
		{
			auto& offset = item.first;
			auto itr = other._informations.find(offset);
			if (itr == other._informations.end())
			{
				return false;
			}

			if (item.second.bytes != itr->second.bytes ||
				item.second.hash != itr->second.hash)
			{
				return false;
			}
		}

		return true;
	}
}
