#include "pybind11/pybind11.h"
#include "blrnaBase.h"
#include "blrnaObject.h"
#include "blrnaImage.h"
#include "Nodes.h"
#include "intermObjectIDMapper.h"
#include "intermImageMapper.h"
#include "intermNameIDMapper.h"
#include "intermNodeParamsFixer.h"

namespace interm
{

	NodeParamsFixer::NodeParamsFixer(const std::unordered_map<void*, int>& instanceIDMap, const ObjectIDMapper& objectIDMapper, ImageMapper& imageMapper, NameIDMapper& nameIDMapper) :
		_instanceIDMap(instanceIDMap), _objectIDMapper(objectIDMapper), _imageMapper(imageMapper), _nameIDMapper(nameIDMapper)
	{
	}

	void NodeParamsFixer::ForMapChannelsPerLinesets(std::function<void(std::shared_ptr<Nodes::LineSetNodeToExport>,
		const std::unordered_set<int>&, const std::unordered_set<int>&, const std::unordered_set<std::string>, const std::unordered_set<std::string>&)> func) const
	{
		for (auto& mapChannelsData : _mapChannelsPerLineset)
		{
			auto lineset = mapChannelsData.first;
			auto& mapChannels = mapChannelsData.second;
			if (mapChannels.uv.empty() && mapChannels.color.empty() && mapChannels.uvNames.empty() && mapChannels.colorNames.empty())
			{
				continue;
			}

			func(lineset, mapChannels.uv, mapChannels.color, mapChannels.uvNames, mapChannels.colorNames);
		}
	}


	void NodeParamsFixer::ExportObjectIDs(std::vector<int>& dst, const std::vector<pybind11::object>& src) const
	{
		_objectIDMapper.ExtractIDs(dst, src);
	}

	void NodeParamsFixer::ExportMaterialIDs(std::vector<int>& dst, const std::vector<pybind11::object>& src) const
	{
		dst.clear();
		dst.reserve(src.size());

		for (auto& obj : src)
		{
			auto itr = _instanceIDMap.find(blrna::ConvertToRNAData(obj));
			if (itr != _instanceIDMap.end())
			{
				dst.emplace_back(itr->second);
			}
		}
	}

	void NodeParamsFixer::ExportArray(float& v0, float& v1, const std::array<float, 2>& src)
	{
		v0 = src[0];
		v1 = src[1];
	}

	void NodeParamsFixer::ExportArray(float& v0, float& v1, float& v2, const std::array<float, 3>& src)
	{
		v0 = src[0];
		v1 = src[1];
		v2 = src[2];
	}

	void NodeParamsFixer::FixNode(std::shared_ptr<Nodes::LineNodeToExport> node)
	{
		if (!node) return;

		node->LineSizeType = static_cast<int>(node->LineSizeTypeValue);
		for (auto lineset : node->LineSetNodesToExport)
		{
			FixNode(lineset);
		}
	}

	void NodeParamsFixer::FixNode(std::shared_ptr<Nodes::LineSetNodeToExport> node)
	{
		if (!node) return;

		if (_mapChannelsPerLineset.find(node) != _mapChannelsPerLineset.end())
		{
			return;
		}
		auto& mapChannels = _mapChannelsPerLineset.emplace(node, MapChannels()).first->second;

		ExportObjectIDs(node->ObjectIds, node->ObjectsToExport);
		ExportMaterialIDs(node->MaterialIds, node->MaterialsToExport);

		FixNode(node->VBrushSettingsToExport, mapChannels);
		FixNode(node->VOutlineToExport, mapChannels);
		FixNode(node->VObjectToExport, mapChannels);
		FixNode(node->VIntersectionToExport, mapChannels);
		FixNode(node->VSmoothToExport, mapChannels);
		FixNode(node->VMaterialToExport, mapChannels);
		FixNode(node->VNormalAngleToExport, mapChannels);
		FixNode(node->VWireframeToExport, mapChannels);
		FixNode(node->VSelectedToExport, mapChannels);
		FixNode(node->VSizeReductionToExport);
		FixNode(node->VAlphaReductionToExport);

		FixNode(node->HBrushSettingsToExport, mapChannels);
		FixNode(node->HOutlineToExport, mapChannels);
		FixNode(node->HObjectToExport, mapChannels);
		FixNode(node->HIntersectionToExport, mapChannels);
		FixNode(node->HSmoothToExport, mapChannels);
		FixNode(node->HMaterialToExport, mapChannels);
		FixNode(node->HNormalAngleToExport, mapChannels);
		FixNode(node->HWireframeToExport, mapChannels);
		FixNode(node->HSelectedToExport, mapChannels);
		FixNode(node->HSizeReductionToExport);
		FixNode(node->HAlphaReductionToExport);

		ExportArray(node->UserDefinedColorR, node->UserDefinedColorG, node->UserDefinedColorB, node->UserDefinedColorRGB);
	}

	void NodeParamsFixer::FixNode(std::shared_ptr<Nodes::BrushSettingsNodeToExport> node, MapChannels& mapChannels) const
	{
		if (!node) return;

		ExportArray(node->ColorR, node->ColorG, node->ColorB, node->ColorRGB);
		FixNode(node->BrushDetailNodeToExport, mapChannels);
		FixNode(node->ColorMapToExport, node->ColorMapOn, mapChannels);
		FixNode(node->SizeMapToExport, node->SizeMapOn, mapChannels);
	}

	void NodeParamsFixer::FixNode(std::shared_ptr<Nodes::BrushDetailNodeToExport> node, MapChannels& mapChannels) const
	{
		if (!node) return;

		node->LoopDirectionType = node->LoopDirectionTypeValue == Nodes::LoopDirectionType::Clockwise ? Pcl4NativeDll::LoopDirection::Right : Pcl4NativeDll::LoopDirection::Left;
		node->ColorModeType = static_cast<int>(node->ColorModeTypeValue);
		FixNode(node->BrushMapToExport, node->BrushMapOn, mapChannels);
		FixNode(node->DistortionMapToExport, node->DistortionMapOn, mapChannels);
	}

	void NodeParamsFixer::FixNode(std::shared_ptr<Nodes::ReductionSettingsNodeToExport> node) const
	{
		if (!node) return;

		if (node->ReferObject && node->ObjectReference.has_value())
		{
			blrna::Object object(node->ObjectReference.value());
			float matrixWorld[16];
			object.get_matrix_world(matrixWorld);

			node->ObjectPosX = matrixWorld[12];
			node->ObjectPosY = matrixWorld[13];
			node->ObjectPosZ = matrixWorld[14];
		}
	}

	void NodeParamsFixer::FixNode(std::shared_ptr<Nodes::TextureMapNodeToExport> node, MapChannels& mapChannels) const
	{
		if (!node) return;

		ExportArray(node->tilingX, node->tilingY, node->tillingXY);
		ExportArray(node->offsetX, node->offsetY, node->offsetXY);
		node->objectColorMode = node->source_type == Nodes::TextureMapSourceType::ObjectColor;

		if (node->objectColorMode)
		{
			if (node->object_color_selection_mode == Nodes::TextureMapSelectionMode::Index)
			{
				node->exChannelIndex = node->object_color_index;
				node->exChannelID = 0;
				mapChannels.color.emplace(node->exChannelIndex);
			}
			else
			{
				node->exChannelIndex = 0;
				node->exChannelID = _nameIDMapper.GetID(node->object_color_name.c_str());
				mapChannels.colorNames.emplace(node->object_color_name);
			}
		}
		else
		{
			node->exChannelID = 0;
			if (node->uv_source == Nodes::TextureMapUVSource::Screen)
			{
				node->uvSource = Pcl4NativeDll::TextureUVSource::Screen;
				node->tilingY = -node->tilingY;
			}
			else if (node->uv_selection_mode == Nodes::TextureMapSelectionMode::Index)
			{
				node->uvSource = static_cast<Pcl4NativeDll::TextureUVSource>((int)Pcl4NativeDll::TextureUVSource::MeshObject1 + node->uv_index);
				mapChannels.uv.emplace(node->uv_index);
			}
			else
			{
				node->uvSource = Pcl4NativeDll::TextureUVSource::MeshObject1;
				node->exChannelID = _nameIDMapper.GetID(node->uv_name.c_str());
				mapChannels.uvNames.emplace(node->uv_name);
			}
		}

		node->ptr_offset = !node->objectColorMode && node->ImageToExport.has_value() ? _imageMapper.GetPtrOffset(node->ImageToExport.value()) : 0;
		if (node->ptr_offset > 0)
		{
			blrna::Image image(node->ImageToExport.value());
			node->width = image.Width();
			node->height = image.Height();
		}
	}

	void NodeParamsFixer::FixNode(std::shared_ptr<Nodes::LineFunctionsNodeToExport> node) const
	{
		if (!node) return;

		ExportMaterialIDs(node->TargetMaterialIdsToSerialize, node->TargetMaterialsToExport);
		ExportObjectIDs(node->DrawObjectIdsToSerialize, node->DrawObjectsToExport);
		ExportMaterialIDs(node->DrawMaterialIdsToSerialize, node->DrawMaterialsToExport);
		ExportObjectIDs(node->MaskObjectIdsToSerialize, node->MaskObjectsToExport);
		ExportMaterialIDs(node->MaskMaterialIdsToSerialize, node->MaskMaterialsToExport);

		ExportArray(node->OutlineColorR, node->OutlineColorG, node->OutlineColorB, node->OutlineColorRGB);
		ExportArray(node->ObjectColorR, node->ObjectColorG, node->ObjectColorB, node->ObjectColorRGB);
		ExportArray(node->IntersectionColorR, node->IntersectionColorG, node->IntersectionColorB, node->IntersectionColorRGB);
		ExportArray(node->SmoothColorR, node->SmoothColorG, node->SmoothColorB, node->SmoothColorRGB);
		ExportArray(node->MaterialColorR, node->MaterialColorG, node->MaterialColorB, node->MaterialColorRGB);
		ExportArray(node->SelectedEdgeColorR, node->SelectedEdgeColorG, node->SelectedEdgeColorB, node->SelectedEdgeColorRGB);
		ExportArray(node->NormalAngleColorR, node->NormalAngleColorG, node->NormalAngleColorB, node->NormalAngleColorRGB);
		ExportArray(node->WireframeColorR, node->WireframeColorG, node->WireframeColorB, node->WireframeColorRGB);
	}

}