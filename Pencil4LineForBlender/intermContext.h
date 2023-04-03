#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include "pybind11/pybind11.h"
#include "Nodes.h"
#include "RenderAppDefine.h"
namespace py = pybind11;

namespace blrna
{
	class Image;
	class Material;
}

namespace interm
{
	class Camera;
	class MeshDataAccessor;
	class CurveData;
	class DataHash;
	struct DrawOptions;

	class Context
	{
	public:
		Context() {}
		~Context() {}
		RenderAppRet Draw(blrna::Image image,
			Camera camera,
			const std::vector<py::object>& renderInstances,
			const py::object materialOverride,
			const std::vector<std::pair<py::object, py::object>>& curveData,
			const std::vector<std::shared_ptr<Nodes::LineNodeToExport>>& lineNodes,
			const std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>>& lineFunctions,
			const std::vector<std::shared_ptr<Nodes::LineRenderElementToExport>>& lineRenderElements,
			const std::vector<std::vector<py::object>>& groups);
		RenderAppRet DrawForViewport(int width, int height,
			Camera camera,
			const std::vector<py::object>& renderInstances,
			const py::object materialOverride,
			const std::vector<std::pair<py::object, py::object>>& curveData,
			const std::vector<std::shared_ptr<Nodes::LineNodeToExport>>& lineNodes,
			const std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>>& lineFunctions,
			const std::vector<std::vector<py::object>>& groups);
		void CleanupFrame();
		void CleanupAll();

		int PointerToInstanceID(void* p);

		std::wstring renderAppPath;
		std::wstring taskName;
		std::shared_ptr<DrawOptions> drawOptions;
		
		bool mesh_color_attributes_on = false;
		std::vector<std::pair<py::object, std::vector<py::object>>> mesh_color_attributes;

		std::shared_ptr<std::vector<unsigned char>> GetViewportImageBuffer() const { return _viewport_image_buffer; }
		void ClearViewportImageBuffer() { _viewport_image_buffer = nullptr; }

	private:
		RenderAppRet DrawImpl(int width, int height,
			blrna::Image image,
			Camera camera,
			const std::array<float, 2>& viewportScale,
			const std::vector<py::object>& renderInstances,
			const py::object materialOverride,
			const std::vector<std::pair<py::object, py::object>>& curveData,
			const std::vector<std::shared_ptr<Nodes::LineNodeToExport>>& lineNodes,
			const std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>>& lineFunctions,
			const std::vector<std::shared_ptr<Nodes::LineRenderElementToExport>>& lineRenderElements,
			const std::vector<std::vector<py::object>>& groups);

		std::unordered_map<void*, int> _instanceIDMap;

		std::vector<float> _textureWorkBuffer;

		std::shared_ptr<std::vector<unsigned char>> _viewport_image_buffer;
		std::shared_ptr<DataHash> _data_hash;
		std::shared_ptr<DataHash> _data_hash_excluding_objects;
	};
}
