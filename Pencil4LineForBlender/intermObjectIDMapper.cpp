#include "pybind11/pybind11.h"
#include "blrnaBase.h"
#include "RenderAppData.h"
#include "intermRenderInstance.h"
#include "intermObjectIDMapper.h"

namespace interm
{
	ObjectIDMapper::ObjectIDMapper(const std::vector<RenderApp::RenderInstance>& renderInstances, const std::vector<pybind11::object>& srcRIObjects)
	{
		_riIDs.resize(srcRIObjects.size());

		_srcCountsAndOffsets.reserve(srcRIObjects.size());

		for (auto& srcRIObject : srcRIObjects)
		{
			const auto& srcRI = *srcRIObject.cast<RenderInstance*>();
			auto srcData = blrna::ConvertToRNAData(srcRI.SrcObject());
			auto emplaceRet = _srcCountsAndOffsets.emplace(srcData, CountAndOffset());
			emplaceRet.first->second.count++;
		}

		std::unordered_map<void*, int> writeOffsets;
		writeOffsets.reserve(_srcCountsAndOffsets.size());
		int offset = 0;
		for (auto& elem : _srcCountsAndOffsets)
		{
			auto& countsAndOffsets = elem.second;

			countsAndOffsets.offset = offset;
			writeOffsets.emplace(elem.first, offset);

			offset += countsAndOffsets.count;
		}

		for (int i = 0; i < (int)renderInstances.size(); i++)
		{
			const auto& srcRI = *srcRIObjects[i].cast<RenderInstance*>();
			auto srcData = blrna::ConvertToRNAData(srcRI.SrcObject());
			auto& writeOffset = writeOffsets[srcData];
			_riIDs[writeOffset++] = renderInstances[i].instanceId;
		}
	}

	void ObjectIDMapper::ExtractIDs(std::vector<int>& dst, const std::vector<pybind11::object>& srcObjcts) const
	{
		dst.clear();

		int count = 0;
		for (auto& srcObject : srcObjcts)
		{
			auto itr = _srcCountsAndOffsets.find(blrna::ConvertToRNAData(srcObject));
			if (itr != _srcCountsAndOffsets.end())
			{
				count += itr->second.count;
			}
		}

		dst.reserve(count);

		for (auto& srcObject : srcObjcts)
		{
			auto itr = _srcCountsAndOffsets.find(blrna::ConvertToRNAData(srcObject));
			if (itr != _srcCountsAndOffsets.end())
			{
				for (int i = 0; i < itr->second.count; i++)
				{
					dst.emplace_back(_riIDs[itr->second.offset + i]);
				}
			}
		}
	}
}
