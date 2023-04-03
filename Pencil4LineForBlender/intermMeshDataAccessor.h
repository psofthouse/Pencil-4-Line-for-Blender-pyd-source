#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "blrnaMesh.h"
#include "RenderAppData.h"

namespace blrna
{
	class Mesh;
	class Material;
}

namespace interm
{
	class Context;
	class NameIDMapper;

	template<class T> class GeomDataAccessor
	{
	public:
		virtual T operator[](size_t i) const = 0;
	};

	class MeshDataAccessor
	{
	public:
		MeshDataAccessor(blrna::Mesh mesh,
			Context& context,
			const blrna::Material& materialOverride,
			const std::vector<py::object>& meshColorAttributes);
		
		void ActivateUVChannel(int channel);
		void ActivateColorChannel(int channel);
		void ActivateUVChannel(const std::string& name, NameIDMapper& nameIDMapper);
		void ActivateColorChannel(const std::string& name, NameIDMapper& nameIDMapper);

		virtual void WriteDataToBuffer(char* buff);

		RenderApp::MeshDataInfo info;

		static std::vector<py::object> color_attributes_default;
		static std::vector<py::object> color_attributes_none;

	protected:
		MeshDataAccessor(blrna::Mesh mesh, const std::vector<py::object>& meshColorAttributes):_mesh(mesh), _meshColorAttributes(meshColorAttributes) {}

		blrna::Mesh _mesh;
		std::vector<int> _mtlFaceFlags;
		const std::vector<py::object>& _meshColorAttributes;
		std::unique_ptr<std::unordered_map<std::string, int>> _uvChannelNames;
		std::unique_ptr<std::unordered_map<std::string, int>> _colorChannelNames;

		std::unique_ptr<GeomDataAccessor<int>> _cornerVertAccessor;
		std::unique_ptr<GeomDataAccessor<int>> _cornerEdgeAccessor;
		std::unique_ptr<GeomDataAccessor<bool>> _edgeSharpAccessor;
		std::unique_ptr<GeomDataAccessor<bool>> _edgeWireAccessor;
		std::unique_ptr<GeomDataAccessor<std::pair<int, int>>> _polyLoopAccessor;
		std::unique_ptr<GeomDataAccessor<bool>> _polySharpAccessor;
		std::unique_ptr<GeomDataAccessor<int>> _polyMaterialIndexAccessor;

		void WriteVerticesData(char* buff);
		void WriteFaceData(char* buff, const std::vector<int>& loopTriIndexToOutputOffset);
		void WriteUVData(char* buff, const std::vector<int>& loopTriIndexToOutputOffset);
		void WriteVertColorData(char* buff, const std::vector<int>& loopTriIndexToOutputOffset);

		void SetupEdgesFlags(std::vector<unsigned char>& edgeFlags);
	};

	class CurveData
	{
	public:
		CurveData(const std::vector<py::object>& materials, const std::vector<int> splineMaterialIndices) :
			_materials(ConvertPyObjectToPtr(materials)), _splineMaterialIndices(splineMaterialIndices)
		{}

		const std::vector<void*>& Materials() const { return _materials; }
		const std::vector<int>& SplineMaterialIndices() const { return _splineMaterialIndices; }

	private:
		static std::vector<void*> ConvertPyObjectToPtr(const std::vector<py::object>& materials);

		const std::vector<void*> _materials;
		const std::vector<int> _splineMaterialIndices;
	};

	class MeshDataAccessorForCurve : public MeshDataAccessor
	{
	public:
		MeshDataAccessorForCurve(blrna::Mesh mesh,
			Context& context,
			const blrna::Material& materialOverride,
			const std::vector<py::object>& meshColorAttributes,
			const CurveData& curveData);

		virtual void WriteDataToBuffer(char* buff);

	private:
		std::vector<std::vector<int>> _subMeshEdges;
	};
}