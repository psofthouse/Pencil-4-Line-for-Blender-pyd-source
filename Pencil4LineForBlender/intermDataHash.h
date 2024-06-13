#pragma once
#include <unordered_map>

namespace interm
{
	class DataHash
	{
	public:
		DataHash() {}
		DataHash(const DataHash& other):_informations(other._informations) {}


		void Record(std::size_t offset, std::size_t bytes, const void* pData);
		bool operator==(const DataHash& other) const;

	private:
		struct Info
		{
			std::size_t bytes;
			long long hash;
		};

		std::unordered_map<std::size_t, Info> _informations;
	};
}

