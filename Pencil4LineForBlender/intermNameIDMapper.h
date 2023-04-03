#pragma once
#include <string>
#include <unordered_map>

namespace interm
{
	class NameIDMapper
	{
	public:
		int GetID(const char* name);

	private:
		std::unordered_map<std::string, int> _map;
	};
}
