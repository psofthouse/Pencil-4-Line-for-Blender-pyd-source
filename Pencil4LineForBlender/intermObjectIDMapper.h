#pragma once
#include <vector>
#include <unordered_map>

namespace RenderApp
{
	struct RenderInstance;
}
namespace pybind11
{
	class object;
}

namespace interm
{
	// ジオメトリノードを使用した場合など、単一のsrcから複数のrenderInstanceが生成され得るので
	// src -> 複数のinstanceId()の情報を管理する
	class ObjectIDMapper
	{
	public:
		ObjectIDMapper(const std::vector<RenderApp::RenderInstance>& renderInstances, const std::vector<pybind11::object>& srcRIObjects);
		void ExtractIDs(std::vector<int>& dst, const std::vector<pybind11::object>& srcObjcts) const;

	private:
		struct CountAndOffset
		{
			int count = 0;
			int offset = 0;
		};
		std::vector<int> _riIDs;
		std::unordered_map<void*, CountAndOffset> _srcCountsAndOffsets;
	};
}
