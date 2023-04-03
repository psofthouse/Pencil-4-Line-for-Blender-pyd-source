#pragma once
#include <unordered_map>

namespace interm
{
	class DataHash
	{
	public:
		DataHash() {}
		DataHash(const DataHash& other):_informations(other._informations) {}


		void Record(size_t offset, size_t bytes, const void* pData);
		bool operator==(const DataHash& other) const;

	private:
		struct Info
		{
			size_t bytes;
			long long hash;
		};

		std::unordered_map<size_t, Info> _informations;
	};
}

