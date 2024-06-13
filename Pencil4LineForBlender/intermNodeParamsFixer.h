#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>

namespace Nodes
{
	struct LineNodeToExport;
	struct LineSetNodeToExport;
	struct BrushSettingsNodeToExport;
	struct BrushDetailNodeToExport;
	struct ReductionSettingsNodeToExport;
	struct TextureMapNodeToExport;
}
namespace pybind11
{
	class object;
}


namespace interm
{
	class ObjectIDMapper;
	class ImageMapper;
	class NameIDMapper;

	class NodeParamsFixer
	{
		struct MapChannels;

	public:
		NodeParamsFixer(const std::unordered_map<void*, int>& instanceIDMap, const ObjectIDMapper& objectIDMapper, ImageMapper& imageMapper, NameIDMapper& nameIDMapper);

		void FixNode(std::shared_ptr<Nodes::LineNodeToExport> node);
		void FixNode(std::shared_ptr<Nodes::LineSetNodeToExport> node);
		void FixNode(std::shared_ptr<Nodes::BrushSettingsNodeToExport> node, MapChannels& mapChannels) const;
		void FixNode(std::shared_ptr<Nodes::BrushDetailNodeToExport> node, MapChannels& mapChannels) const;
		void FixNode(std::shared_ptr<Nodes::BrushDetailNodeToExport> node) const { MapChannels mapChannels; FixNode(node, mapChannels); }
		void FixNode(std::shared_ptr<Nodes::ReductionSettingsNodeToExport> node) const;
		void FixNode(std::shared_ptr<Nodes::TextureMapNodeToExport> node, MapChannels& mapChannels) const;
		void FixNode(std::shared_ptr<Nodes::LineFunctionsNodeToExport> node) const;

		void ForMapChannelsPerLinesets(std::function<void(std::shared_ptr<Nodes::LineSetNodeToExport>,
			const std::unordered_set<int>&, const std::unordered_set<int>&, const std::unordered_set<std::string>, const std::unordered_set<std::string>&)> func) const;

	private:
		template<class T> void FixNode(T& node, bool on) const
		{
			if (on && node)
			{
				FixNode(node);
			}
			else
			{
				node = nullptr;
			}
		}
		template<class T> void FixNode(T& node, bool on, MapChannels& mapChannels) const
		{
			if (on && node)
			{
				FixNode(node, mapChannels);
			}
			else
			{
				node = nullptr;
			}
		}

		void ExportObjectIDs(std::vector<int>& dst, const std::vector<pybind11::object>& src) const;
		void ExportMaterialIDs(std::vector<int>& dst, const std::vector<pybind11::object>& src) const;
		static void ExportArray(float& v0, float& v1, const std::array<float, 2>& src);
		static void ExportArray(float& v0, float& v1, float& v2, const std::array<float, 3>& src);

		const std::unordered_map<void*, int>& _instanceIDMap;
		const ObjectIDMapper& _objectIDMapper;
		ImageMapper& _imageMapper;
		NameIDMapper& _nameIDMapper;

		struct MapChannels
		{
			std::unordered_set<int> uv;
			std::unordered_set<int> color;
			std::unordered_set<std::string> uvNames;
			std::unordered_set<std::string> colorNames;
		};
		std::unordered_map<std::shared_ptr<Nodes::LineSetNodeToExport>, MapChannels> _mapChannelsPerLineset;
	};

}