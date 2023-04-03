#include <numeric>
#include <functional>

#include "intermMeshDataAccessor.h"
#include "intermNameIDMapper.h"
#include "intermBlenderVersion.h"

#include "blrnaMLoop.h"
#include "blrnaMEdge.h"
#include "blrnaMPoly.h"
#include "blrnaMaterial.h"
#include "blrnaMeshUVLoopLayer.h"
#include "blrnaMeshLoopColorLayer.h"
#include "blrnaIDOverrideLibrary.h"
#include "blrnaAttribute.h"
#include "DNA_meshdata_types.h"

#include "intermContext.h"

namespace interm
{
	extern float srgb_to_linear_float(float x);

	std::vector<py::object> MeshDataAccessor::color_attributes_default;
	std::vector<py::object> MeshDataAccessor::color_attributes_none;

	static void* GetMaterialOriginalData(const blrna::Material& material)
	{
		auto org_material = material.original<blrna::Material>();

		auto ptr = org_material.override_library_data();
		if (ptr)
		{
			static StructRNA* g_idOverrideLibraryItemType = material.override_library_item_type();
			blrna::IDOverrideLibrary materialObjectWork(PointerRNA{ nullptr, g_idOverrideLibraryItemType, ptr });
			return materialObjectWork.get_reference().data;
		}

		return org_material.data();
	}

	//-----------------------------------------
	template <class T, class blrnaT> class GeomAttribAccessor : public GeomDataAccessor<T>
	{
	public:
		GeomAttribAccessor(PointerRNA prna) : _data(blrnaT(prna).get_data<T*>()) {}
		T operator[](size_t i) const override { return _data[i]; }
	private:
		const T* _data;
	};
	typedef GeomAttribAccessor<int, blrna::IntAttribute> GeomAttribIntAccessor;
	typedef GeomAttribAccessor<bool, blrna::BoolAttribute> GeomAttribBoolAccessor;

	template <class T> class DefaultDataAccessor : public GeomDataAccessor<T>
	{
	public:
		DefaultDataAccessor(const T& value) :_value(value) {}
		T operator[](size_t i) const override { return _value; }
	private:
		T _value;
	};

	class GeomCornerVertAccessorMLoop : public GeomDataAccessor<int>
	{
	public:
		GeomCornerVertAccessorMLoop(blrna::Mesh mesh) : _pLoops(mesh.get_loops<MLoop*>()) {}
		int operator[](size_t i) const override { return _pLoops[i].v; }
	private:
		const MLoop* _pLoops;
	};

	class GeomCornerEdgeAccessorMLoop : public GeomDataAccessor<int>
	{
	public:
		GeomCornerEdgeAccessorMLoop(blrna::Mesh mesh) : _pLoops(mesh.get_loops<MLoop*>()) {}
		int operator[](size_t i) const override { return _pLoops[i].e; }
	private:
		const MLoop* _pLoops;
	};

	class GeomEdgeSharpAccessorMEdge : public GeomDataAccessor<bool>
	{
	public:
		GeomEdgeSharpAccessorMEdge(blrna::Mesh mesh) : _pEdges(mesh.get_edges<MEdge*>()) {}
		bool operator[](size_t i) const override { return _pEdges[i].flag & ME_SHARP; }
	private:
		const MEdge* _pEdges;
	};

	class GeomEdgeWireAccessorMEdge : public GeomDataAccessor<bool>
	{
	public:
		GeomEdgeWireAccessorMEdge(blrna::Mesh mesh) : _pEdges(mesh.get_edges<MEdge*>()) {}
		bool operator[](size_t i) const override { return _pEdges[i].flag & (ME_EDGEDRAW | ME_EDGERENDER); }
	private:
		const MEdge* _pEdges;
	};

	class GeomPolyLoopAccessorMPloy : public GeomDataAccessor<std::pair<int, int>>
	{
	public:
		GeomPolyLoopAccessorMPloy(blrna::Mesh mesh) : _pPolygons(mesh.get_polygons<MPoly*>()) {}
		std::pair<int, int> operator[](size_t i) const override { return std::make_pair(_pPolygons[i].loopstart, _pPolygons[i].loopstart + _pPolygons[i].totloop); }
	private:
		const MPoly* _pPolygons;
	};

	class GeomPolySharpAccessorMPloy : public GeomDataAccessor<bool>
	{
	public:
		GeomPolySharpAccessorMPloy(blrna::Mesh mesh) : _pPolygons(mesh.get_polygons<MPoly*>()) {}
		bool operator[](size_t i) const override { return !(_pPolygons[i].flag & ME_SMOOTH); }
	private:
		const MPoly* _pPolygons;
	};

	class GeomPolyMaterialIndexAccessorMPloy : public GeomDataAccessor<int>
	{
	public:
		GeomPolyMaterialIndexAccessorMPloy(blrna::Mesh mesh) : _pPolygons(mesh.get_polygons<MPoly*>())
		, _mpolyObjectWork(PointerRNA{ (ID*)mesh.data(), mesh.polygons_item_type(), nullptr })  {}
		int operator[](size_t i) const override { (const_cast<blrna::MPoly*>(&_mpolyObjectWork))->SetMPoly(&(_pPolygons[i])); return _mpolyObjectWork.get_material_index(); }
	private:
		const MPoly* _pPolygons;
		blrna::MPoly _mpolyObjectWork;
	};

	class GeomPolyLoopAccessorArray : public GeomDataAccessor<std::pair<int, int>>
	{
	public:
		GeomPolyLoopAccessorArray(blrna::Mesh mesh) : _pData(mesh.get_polygons<int*>()) {}
		std::pair<int, int> operator[](size_t i) const override { return std::make_pair(_pData[i], _pData[i + 1]); }
	private:
		const int* _pData;
	};

	class GeomPolyMaterialIndexAccessorAttrb : public GeomDataAccessor<int>
	{
	public:
		GeomPolyMaterialIndexAccessorAttrb(PointerRNA prna, blrna::Mesh mesh) : _attribAccessor(prna), _max(std::max(mesh.get_materials_length() - 1, 0)) {}
		int operator[](size_t i) const override { return std::min(std::max(_attribAccessor[i], 0), _max); }
	private:
		const GeomAttribIntAccessor _attribAccessor;
		const int _max;
	};

	class GeomData
	{
	public:
		static std::unique_ptr<GeomDataAccessor<int>> CornerVertAccessor(blrna::Mesh mesh)
		{
			return FetchMeshCornerAccessor<GeomDataAccessor<int>, GeomCornerVertAccessorMLoop, GeomAttribIntAccessor>(mesh, ".corner_vert");
		}
		static std::unique_ptr<GeomDataAccessor<int>> CornerEdgeAccessor(blrna::Mesh mesh)
		{
			return FetchMeshCornerAccessor<GeomDataAccessor<int>, GeomCornerEdgeAccessorMLoop, GeomAttribIntAccessor>(mesh, ".corner_edge");
		}
		static std::unique_ptr<GeomDataAccessor<bool>> EdgeSharpAccessor(blrna::Mesh mesh)
		{
			if (BlenderVersion::Shared() >= BlenderVersion(3, 5))
			{
				std::unique_ptr<GeomDataAccessor<bool>> ret = std::make_unique<DefaultDataAccessor<bool>>(false);
				mesh.foreach_attributes([&mesh, &ret](const CollectionPropertyIterator& itr) {
					if (blrna::Attribute::Equals(itr.ptr, blrna::Attribute::AttrDomain::EDGE, blrna::Attribute::DataType::BOOL, "sharp_edge"))
					{
						ret = std::make_unique<GeomAttribBoolAccessor>(itr.ptr);
					}
				});
				return ret;
			}
			else
			{
				return std::make_unique<GeomEdgeSharpAccessorMEdge>(mesh);
			}
		}
		static std::unique_ptr<GeomDataAccessor<bool>> EdgeWireAccessor(blrna::Mesh mesh)
		{
			std::unique_ptr<GeomDataAccessor<bool>> ret;
			mesh.foreach_attributes([&mesh, &ret](const CollectionPropertyIterator& itr) {
				if (blrna::Attribute::Equals(itr.ptr, blrna::Attribute::AttrDomain::EDGE, blrna::Attribute::DataType::BOOL, "pencil_wire_edge"))
				{
					ret = std::make_unique<GeomAttribBoolAccessor>(itr.ptr);
				}
			});
			if (!ret)
			{
				if (BlenderVersion::Shared() >= BlenderVersion(3, 5))
				{
					ret = std::make_unique<DefaultDataAccessor<bool>>(true);
				}
				else
				{
					ret = std::make_unique<GeomEdgeWireAccessorMEdge>(mesh);
				}
			}
			return ret;
		}
		static std::unique_ptr<GeomDataAccessor<std::pair<int, int>>> PolyLoopAccessor(blrna::Mesh mesh)
		{
			if (BlenderVersion::Shared() >= BlenderVersion(3, 6))
			{
				return std::make_unique<GeomPolyLoopAccessorArray>(mesh);
			}
			else
			{
				return std::make_unique<GeomPolyLoopAccessorMPloy>(mesh);
			}
		}
		static std::unique_ptr<GeomDataAccessor<bool>> PolySharpAccessor(blrna::Mesh mesh)
		{
			if (BlenderVersion::Shared() >= BlenderVersion(3, 6))
			{
				std::unique_ptr<GeomDataAccessor<bool>> ret = std::make_unique<DefaultDataAccessor<bool>>(false);
				mesh.foreach_attributes([&mesh, &ret](const CollectionPropertyIterator& itr) {
					if (blrna::Attribute::Equals(itr.ptr, blrna::Attribute::AttrDomain::FACE, blrna::Attribute::DataType::BOOL, "sharp_face"))
					{
						ret = std::make_unique<GeomAttribBoolAccessor>(itr.ptr);
					}
				});
				return ret;
			}
			else
			{
				return std::make_unique<GeomPolySharpAccessorMPloy>(mesh);
			}
		}
		static std::unique_ptr<GeomDataAccessor<int>> PolyMaterialIndexAccessor(blrna::Mesh mesh)
		{
			if (BlenderVersion::Shared() >= BlenderVersion(3, 6))
			{
				std::unique_ptr<GeomDataAccessor<int>> ret = std::make_unique<DefaultDataAccessor<int>>(0);
				mesh.foreach_attributes([&mesh, &ret](const CollectionPropertyIterator& itr) {
					if (blrna::Attribute::Equals(itr.ptr, blrna::Attribute::AttrDomain::FACE, blrna::Attribute::DataType::INT32, "material_index"))
					{
						ret = std::make_unique<GeomPolyMaterialIndexAccessorAttrb>(itr.ptr, mesh);
					}
				});
				return ret;
			}
			else
			{
				return std::make_unique<GeomPolyMaterialIndexAccessorMPloy>(mesh);
			}
		}

	private:
		template<class TBase, class TMLoop, class TAttrib> std::unique_ptr<TBase> static FetchMeshCornerAccessor(blrna::Mesh mesh, const char* attribName)
		{
			std::unique_ptr<TBase> ret = std::make_unique<TMLoop>(mesh);
			if (mesh.get_loops_data_step<MLoop>() == sizeof(unsigned int))
			{
				mesh.foreach_attributes([&mesh, &ret, attribName](const CollectionPropertyIterator& itr) {
					if (itr.ptr.data)
					{
						blrna::Attribute attrib(itr.ptr);
						auto name = attrib.get_struct_name();
						if (name == attribName)
						{
							ret = std::make_unique<TAttrib>(itr.ptr);
						}
					}
				});
			}
			return ret;
		}
	};

	//-----------------------------------------
	MeshDataAccessor::MeshDataAccessor(blrna::Mesh mesh,
		Context& context,
		const blrna::Material& materialOverride,
		const std::vector<py::object>& meshColorAttributes) : _mesh(mesh), _meshColorAttributes(meshColorAttributes)
	{
		if (mesh.get_polygons_length() > 0)
		{
			mesh.calc_loop_triangles();

			_cornerVertAccessor = GeomData::CornerVertAccessor(_mesh);
			_cornerEdgeAccessor = GeomData::CornerEdgeAccessor(_mesh);
			_edgeSharpAccessor = GeomData::EdgeSharpAccessor(_mesh);
			_edgeWireAccessor = GeomData::EdgeWireAccessor(_mesh);
			_polyLoopAccessor = GeomData::PolyLoopAccessor(_mesh);
			_polySharpAccessor = GeomData::PolySharpAccessor(_mesh);
			_polyMaterialIndexAccessor = GeomData::PolyMaterialIndexAccessor(_mesh);
			auto& materialIndices = *_polyMaterialIndexAccessor;

			// メッシュの基本情報を設定
			info.vertexCount = mesh.get_vertices_length();
			info.triangleFlagsEnabled = true;
			info.objectType = 0;

			// サブメッシュの数（使用しているマテリアル数）、サブメッシュごとのトライアングル数をカウント
			const auto ltPtr = mesh.get_loop_triangles<MLoopTri*>();
			const auto mumMaterials = mesh.get_materials_length();
			const auto materialsPtr = mesh.get_materials<void**>();

			info.subMeshPrimitiveCounts.resize(std::max(mumMaterials, 1), 0);
			auto numTri = mesh.get_loop_triangles_length();
			if (info.subMeshPrimitiveCounts.size() > 1)
			{
				for (int i = 0; i < numTri; i++)
				{
					info.subMeshPrimitiveCounts[materialIndices[(size_t)ltPtr[i].poly]]++;
				}
			}
			else
			{
				info.subMeshPrimitiveCounts[0] = numTri;
			}

			// マテリアルのID設定と、マテリアルの裏面カリングとしてのセットアップ
			info.subMeshMaterialIds.resize(info.subMeshPrimitiveCounts.size(), -1);

			if (mumMaterials > 0)
			{
				auto materialItemType = _mesh.materials_item_type();
				blrna::Material materialObjectWork(PointerRNA{ (ID*)_mesh.data(), materialItemType, nullptr });

				_mtlFaceFlags.reserve(mumMaterials);
				for (int i = 0; i < mumMaterials; i++)
				{
					auto pMaterial = materialsPtr[i];
					if (pMaterial)
					{
						materialObjectWork.SetData(pMaterial);
						auto material = materialOverride ? materialOverride : materialObjectWork;
						_mtlFaceFlags.emplace_back(material.get_use_backface_culling() ? 0 : RenderApp::FaceFlag_DoubleSided);
						info.subMeshMaterialIds[i] = context.PointerToInstanceID(GetMaterialOriginalData(material));
					}
					else
					{
						_mtlFaceFlags.emplace_back(RenderApp::FaceFlag_DoubleSided);
					}
				}
			}
			else
			{
				_mtlFaceFlags.emplace_back(RenderApp::FaceFlag_DoubleSided);
			}
		}
	}

	MeshDataAccessorForCurve::MeshDataAccessorForCurve(blrna::Mesh mesh,
		Context& context,
		const blrna::Material& materialOverride,
		const std::vector<py::object>& meshColorAttributes,
		const CurveData& curveData) : MeshDataAccessor(mesh, meshColorAttributes)
	{
		info.vertexCount = mesh.get_vertices_length();
		info.triangleFlagsEnabled = false;
		info.objectType = 1;

		auto& materials = curveData.Materials();
		auto& splineMaterialIndices = curveData.SplineMaterialIndices();
		const auto numEdges = _mesh.get_edges_length();
		const auto pEdges = _mesh.get_edges<MEdge*>();

		// スプラインをサブメッシュに振り分ける
		_subMeshEdges.resize(std::max(materials.size(), (size_t)1));
		for (auto& edges : _subMeshEdges)
		{
			edges.reserve(numEdges);
		}

		if (_subMeshEdges.size() == 1)
		{
			_subMeshEdges[0].resize(numEdges);
			std::iota(_subMeshEdges[0].begin(), _subMeshEdges[0].end(), 0);
		}
		else
		{
			int prevEdgeV2 = numEdges > 0 ? pEdges[0].v1 : 0;
			int splineIndex = 0;
			for (int i = 0; i < numEdges; i++)
			{
				const auto& edge = pEdges[i];
				if (prevEdgeV2 != edge.v1 && prevEdgeV2 != edge.v2)
				{
					splineIndex++;
					if (splineIndex == splineMaterialIndices.size())
					{
						break;
					}
				}

				int subMesh = splineMaterialIndices[splineIndex];

				_subMeshEdges[subMesh].emplace_back(i);
				prevEdgeV2 = edge.v2;
			}
		}

		info.subMeshPrimitiveCounts.reserve(_subMeshEdges.size());
		for (auto& edges : _subMeshEdges)
		{
			info.subMeshPrimitiveCounts.emplace_back((int)edges.size());
		}

		// マテリアル設定
		info.subMeshMaterialIds.resize(_subMeshEdges.size(), -1);
		auto materialItemType = _mesh.materials_item_type();
		blrna::Material materialObjectWork(PointerRNA{ (ID*)_mesh.data(), materialItemType, nullptr });
		for (int i = 0; i < (int)materials.size(); i++)
		{
			auto pMaterial = materials[i];
			if (pMaterial)
			{
				materialObjectWork.SetData(pMaterial);
				auto material = materialOverride ? materialOverride : materialObjectWork;
				pMaterial = GetMaterialOriginalData(material);
			}
			info.subMeshMaterialIds[i] = context.PointerToInstanceID(pMaterial);
		}
	}

	void MeshDataAccessor::ActivateUVChannel(int channel)
	{
		if (0 <= channel && channel < _mesh.get_uv_layers_length() && _mesh.uv_layers_lookup_int(channel).data)
		{
			auto& v = info.uvChannels;
			if (std::find(v.begin(), v.end(), channel) == v.end())
			{
				v.emplace_back(channel);
			}
		}
	}

	void MeshDataAccessor::ActivateColorChannel(int channel)
	{
		if (&_meshColorAttributes == &MeshDataAccessor::color_attributes_none)
		{	  
			if (0 <= channel && channel < _mesh.get_vertex_colors_length() && _mesh.vertex_colors_lookup_int(channel).data)
			{
				auto& v = info.faceCornerColorChannels;
				if (std::find(v.begin(), v.end(), channel) == v.end())
				{
					v.emplace_back(channel);
				}
			}
		}
		else
		{
			if (0 <= channel && channel < (int)_meshColorAttributes.size() && blrna::ConvertToRNAData(_meshColorAttributes[channel]))
			{
				blrna::ColorAttribute attr(_meshColorAttributes[channel]);
				auto item = attr.get_domain_item();
				auto& v = strcmp("CORNER", item->identifier) == 0 ? info.faceCornerColorChannels : info.vertColorChannels;
				if (std::find(v.begin(), v.end(), channel) == v.end())
				{
					v.emplace_back(channel);
				}
			}
		}
	}

	typedef void (*ACTIVATE_CHANNEL_CALLBACK)(int, void*);
	static void ActivateUVChannel(int channel, void* data) { static_cast<MeshDataAccessor*>(data)->ActivateUVChannel(channel); }
	static void ActivateColorChannel(int channel, void* data) { static_cast<MeshDataAccessor*>(data)->ActivateColorChannel(channel); }

	void RegisterChannelName(const std::string& name,
		std::unordered_map<std::string, int>& channelNames,
		std::vector<int>& infoChannelIDs,
		ACTIVATE_CHANNEL_CALLBACK activeteChannelCallback,
		void* callbackUserData,
		NameIDMapper& nameIDMapper)
	{
		auto itr = channelNames.find(name);
		if (itr != channelNames.end())
		{
			auto channel = itr->second;
			activeteChannelCallback(channel, callbackUserData);
			if (infoChannelIDs.size() <= channel)
			{
				infoChannelIDs.resize(channel + 1, 0);
			}
			infoChannelIDs[channel] = nameIDMapper.GetID(name.c_str());
		}
	}

	void MeshDataAccessor::ActivateUVChannel(const std::string& name, NameIDMapper& nameIDMapper)
	{
		if (!_uvChannelNames)
		{
			_uvChannelNames = std::make_unique<std::unordered_map<std::string, int>>();
			auto numChannels = _mesh.get_uv_layers_length();
			for (int channel = 0; channel < numChannels; channel++)
			{
				blrna::MeshUVLoopLayer uvLoopLayer(_mesh.uv_layers_lookup_int(channel));
				auto n = uvLoopLayer.get_struct_name();
				_uvChannelNames->emplace(uvLoopLayer.get_struct_name(), channel);
			}
		}

		RegisterChannelName(name, *_uvChannelNames, info.uvChannelIDs, interm::ActivateUVChannel, this, nameIDMapper);
	}

	void MeshDataAccessor::ActivateColorChannel(const std::string& name, NameIDMapper& nameIDMapper)
	{
		if (!_colorChannelNames)
		{
			_colorChannelNames = std::make_unique<std::unordered_map<std::string, int>>();

			if (&_meshColorAttributes == &MeshDataAccessor::color_attributes_none)
			{
				auto numChannels = _mesh.get_vertex_colors_length();
				for (int channel = 0; channel < numChannels; channel++)
				{
					blrna::MeshLoopColorLayer meshLoopColorLayer(_mesh.vertex_colors_lookup_int(channel));
					_colorChannelNames->emplace(meshLoopColorLayer.get_struct_name(), channel);
				}
			}
			else
			{
				auto numChannels = (int)_meshColorAttributes.size();
				for (int channel = 0; channel < numChannels; channel++)
				{
					blrna::ColorAttribute attr(_meshColorAttributes[channel]);
					_colorChannelNames->emplace(attr.get_name(), channel);
				}
			}
		}

		RegisterChannelName(name, *_colorChannelNames, info.colorChannelIDs, interm::ActivateColorChannel, this, nameIDMapper);
	}

	void MeshDataAccessor::WriteVerticesData(char* buff)
	{
		auto pDst = info.VerticesData(buff);
		auto numVerts = _mesh.get_vertices_length();
		auto mverts_data_step = _mesh.get_vertices_data_step<MVert>();
		auto pSrc = _mesh.get_vertices<char*>();
		for (int i = 0; i < numVerts; i++, pSrc += mverts_data_step)
		{
			memcpy(pDst++, pSrc, 3 * sizeof(float));
		};
	}

	void MeshDataAccessor::WriteFaceData(char* buff, const std::vector<int>& loopTriIndexToOutputOffset)
	{
		const auto numTri = info.PrimitiveCountAll();
		const auto ltPtr = _mesh.get_loop_triangles<MLoopTri*>();
		const auto polygonsPtr = _mesh.get_polygons<MPoly*>();
		const auto pEdges = _mesh.get_edges<MEdge*>();
		auto& cornerVerts = *_cornerVertAccessor;
		auto& cornerEdges = *_cornerEdgeAccessor;
		auto& materialIndices = *_polyMaterialIndexAccessor;

		std::vector<unsigned char> edgeFlags;
		SetupEdgesFlags(edgeFlags);

		auto pDstTri = info.IndicesData(buff);
		auto pDstFlag = info.TriangleFlagsData(buff);
		auto isMultiMaterial = info.subMeshPrimitiveCounts.size() > 1;
		int loopEdges[3];
		for (int i = 0; i < numTri; i++)
		{
			const auto offset = loopTriIndexToOutputOffset[i];
			const auto& lt = ltPtr[i];

			auto loopVerts = &pDstTri[3 * offset];
			for (int t = 0; t < 3; t++)
			{
				loopVerts[t] = cornerVerts[lt.tri[t]];
				loopEdges[t] = cornerEdges[lt.tri[t]];
			}

			auto dstFlag = _mtlFaceFlags[isMultiMaterial ? materialIndices[ltPtr[i].poly] : 0];
			for (int t = 2, tNext = 0; tNext < 3; t = tNext++)
			{
				const auto& edgeFlag = edgeFlags[loopEdges[t]];
				if (edgeFlag)
				{
					const auto& edge = pEdges[loopEdges[t]];
					if ((edge.v1 == loopVerts[t] && edge.v2 == loopVerts[tNext]) ||
						(edge.v2 == loopVerts[t] && edge.v1 == loopVerts[tNext])) 
					{
						dstFlag |= edgeFlag << t;
					}
				}
			}
			pDstFlag[offset] = dstFlag;
		};
	}

	void MeshDataAccessor::WriteUVData(char* buff, const std::vector<int>& loopTriIndexToOutputOffset)
	{
		if (info.uvChannels.empty())
		{
			return;
		}

		const auto numTri = info.PrimitiveCountAll();
		const auto ltPtr = _mesh.get_loop_triangles<MLoopTri*>();
		auto pDst = info.UVsData(buff);
		for (int uvChannel : info.uvChannels)
		{
			blrna::MeshUVLoopLayer uvLoopLayer(_mesh.uv_layers_lookup_int(uvChannel));

			if (uvLoopLayer.has_uv_prop())
			{
				const auto pFloat2 = uvLoopLayer.get_uv<float*>();

				for (int i = 0; i < numTri; i++)
				{
					const auto offset = loopTriIndexToOutputOffset[i];
					const auto& lt = ltPtr[i];

					for (int t = 0; t < 3; t++)
					{
						auto pUV = pFloat2 + 2 * lt.tri[t];
						pDst[3 * offset + t].x = pUV[0];
						pDst[3 * offset + t].y = pUV[1];
					}
				};
			}
			else
			{
				const auto pMLoopUV = uvLoopLayer.get_data<MLoopUV*>();

				for (int i = 0; i < numTri; i++)
				{
					const auto offset = loopTriIndexToOutputOffset[i];
					const auto& lt = ltPtr[i];

					for (int t = 0; t < 3; t++)
					{
						auto& loopUV = pMLoopUV[lt.tri[t]];
						pDst[3 * offset + t].x = loopUV.uv[0];
						pDst[3 * offset + t].y = loopUV.uv[1];
					}
				};
			}


			//
			pDst += info.IndexCountAll();
		}
	}

	void MeshDataAccessor::WriteVertColorData(char* buff, const std::vector<int>& loopTriIndexToOutputOffset)
	{
		if (info.vertColorChannels.empty() && info.faceCornerColorChannels.empty())
		{
			return;
		}

		const auto numTri = info.PrimitiveCountAll();
		const auto ltPtr = _mesh.get_loop_triangles<MLoopTri*>();
		auto pDst = info.ColorData(buff);

		if (&_meshColorAttributes == &MeshDataAccessor::color_attributes_none)
		{
			for (int channel : info.faceCornerColorChannels)
			{
				blrna::MeshLoopColorLayer meshLoopColorLayer(_mesh.vertex_colors_lookup_int(channel));
				const auto pMLoopColor = meshLoopColorLayer.get_data<MLoopCol*>();

				for (int i = 0; i < numTri; i++)
				{
					const auto offset = loopTriIndexToOutputOffset[i];
					const auto& lt = ltPtr[i];

					for (int t = 0; t < 3; t++)
					{
						constexpr float coef = 1.0f / 255.0f;
						auto& loopColor = pMLoopColor[lt.tri[t]];
						auto& dst = pDst[3 * offset + t];
						dst.r = srgb_to_linear_float(coef * loopColor.r);
						dst.g = srgb_to_linear_float(coef * loopColor.g);
						dst.b = srgb_to_linear_float(coef * loopColor.b);
						dst.a = coef * loopColor.a;
					}
				};

				//
				pDst += info.IndexCountAll();
			}
		}
		else
		{
			const auto numVerts = info.vertexCount;
			for (int channel : info.vertColorChannels)
			{
				blrna::ColorAttribute attr(_meshColorAttributes[channel]);

				for (int i = 0; i < numVerts; i++)
				{
					attr.get_color(i, &pDst[i].r);
				}

				//
				pDst += numVerts;
			}

			for (int channel : info.faceCornerColorChannels)
			{
				blrna::ColorAttribute attr(_meshColorAttributes[channel]);

				for (int i = 0; i < numTri; i++)
				{
					const auto offset = loopTriIndexToOutputOffset[i];
					const auto& lt = ltPtr[i];

					for (int t = 0; t < 3; t++)
					{
						attr.get_color(lt.tri[t], &pDst[3 * offset + t].r);
					}
				};

				//
				pDst += info.IndexCountAll();
			}
		}
	}

	void MeshDataAccessor::WriteDataToBuffer(char* buff)
	{
		if (info.MeshDataSize() == 0)
		{
			return;
		}

		WriteVerticesData(buff);

		//
		const auto numTri = info.PrimitiveCountAll();
		const auto ltPtr = _mesh.get_loop_triangles<MLoopTri*>();
		const auto polygonsPtr = _mesh.get_polygons<MPoly*>();
		auto& materialIndices = *_polyMaterialIndexAccessor;

		std::vector<int> loopTriIndexToOutputOffset;
		loopTriIndexToOutputOffset.resize(numTri);

		if (info.subMeshPrimitiveCounts.size() > 1)
		{
			std::vector<int> offsets(info.subMeshPrimitiveCounts.size());
			for (int i = 0, offset = 0; i < (int)offsets.size(); offset += info.subMeshPrimitiveCounts[i++])
			{
				offsets[i] = offset;
			}
			for (int i = 0; i < numTri; i++)
			{
				loopTriIndexToOutputOffset[i] = offsets[materialIndices[ltPtr[i].poly]]++;
			}
		}
		else
		{
			std::iota(loopTriIndexToOutputOffset.begin(), loopTriIndexToOutputOffset.end(), 0);
		}

		WriteFaceData(buff, loopTriIndexToOutputOffset);
		WriteUVData(buff, loopTriIndexToOutputOffset);
		WriteVertColorData(buff, loopTriIndexToOutputOffset);
	}

	void MeshDataAccessorForCurve::WriteDataToBuffer(char* buff)
	{
		if (info.MeshDataSize() == 0)
		{
			return;
		}

		WriteVerticesData(buff);

		{
			const auto pEdges = _mesh.get_edges<MEdge*>();
			auto pDstIndices = info.IndicesData(buff);

			int offset = 0;
			for (auto& edges : _subMeshEdges)
			{
				for (auto& edgeIndex : edges)
				{
					const auto& edge = pEdges[edgeIndex];
					pDstIndices[offset * 2 + 0] = edge.v1;
					pDstIndices[offset * 2 + 1] = edge.v2;
					offset++;
				}
			}
		}
	}


	struct EdgeToLoop
	{
		int i[2];

		inline bool isNull() const { return !(i[0] || i[1]); }

		inline void Set(int iPrev, int iCurr, int iNext, const GeomDataAccessor<int>& cornerVerts, const MEdge& edge)
		{
			if (cornerVerts[iCurr] == edge.v1)
			{
				i[0] = iCurr;
				i[1] = cornerVerts[iNext] == edge.v2 ? iNext : iPrev;
			}
			else
			{
				i[1] = iCurr;
				i[0] = cornerVerts[iNext] == edge.v1 ? iNext : iPrev;
			}
		}

		inline void GetNormal(float n[3], int which, const char* pLoops, blrna::MLoop& mloopObjectWork, size_t dataStep) const 
		{
			mloopObjectWork.SetMLoop(pLoops + i[which] * dataStep);
			mloopObjectWork.get_normal(n);
		}
	};

	void MeshDataAccessor::SetupEdgesFlags(std::vector<unsigned char>& edgeFlags)
	{
		const auto numEdges = _mesh.get_edges_length();
		const auto pEdges = _mesh.get_edges<MEdge*>();
		const auto numPolygons = _mesh.get_polygons_length();
		auto& cornerVerts = *_cornerVertAccessor;
		auto& cornerEdges = *_cornerEdgeAccessor;
		auto& edgeSharps = *_edgeSharpAccessor;
		auto& edgeWires = *_edgeWireAccessor;
		auto& polyLoops = *_polyLoopAccessor;
		auto& polySharp = *_polySharpAccessor;

		const bool autoSmooth = _mesh.get_use_auto_smooth();
		const auto autoSmoothAngle = _mesh.get_auto_smooth_angle();
		const bool hasCustomNormal = _mesh.get_has_custom_normals();

		edgeFlags.resize(numEdges);
		{
			auto medgeItemType = _mesh.edges_item_type();
			blrna::MEdge medgeObjectWork(PointerRNA{ (ID*)_mesh.data(), medgeItemType, nullptr });
			const auto EdgeSharp_to_FaceFlag = autoSmooth ? RenderApp::FaceFlag_SmoothingBound0_1 : 0;

			for (int i = 0; i < numEdges; i++)
			{
				auto& edge = pEdges[i];
				medgeObjectWork.SetMEdge(&edge);
				edgeFlags[i] = (edgeWires[i] ? RenderApp::FaceFlag_Wire0_1 : 0) +
					(edgeSharps[i] ? EdgeSharp_to_FaceFlag : 0) +
					(medgeObjectWork.get_use_freestyle_mark() ? RenderApp::FaceFlag_SelectedEdge0_1 : 0);
			}
		}

		// スムージング境界の追加判定
		if (!autoSmooth || hasCustomNormal || autoSmoothAngle >= 3.14159f)
		{
			// 法線角度による判定の必要がない場合は、ポリゴンのME_SMOOTHフラグで判定する
			bool contains_flat_polygon = false;
			for (int pi = 0; pi < numPolygons; pi++)
			{
				if (polySharp[pi])
				{
					contains_flat_polygon = true;
					break;
				}
			}

			if (contains_flat_polygon)
			{
				std::vector<int> edgeToPoly(numEdges, -1);

				for (int pi = 0; pi < numPolygons; pi++)
				{
					const auto loop = polyLoops[pi];
					for (int li = loop.first; li < loop.second; li++)
					{
						auto loop_e = cornerEdges[li];
						auto& etop = edgeToPoly[loop_e];
						if (etop < 0)
						{
							etop = pi;
						}
						else if (polySharp[pi] || polySharp[etop])
						{
							edgeFlags[loop_e] |= RenderApp::FaceFlag_SmoothingBound0_1;
						}
					}
				}
			}
		}
		else 
		{
			// 法線角度による判定が必要な場合、法線を計算させて法線が不一致のエッジをスムージング境界とする
			EdgeToLoop etolTmp;
			std::vector<EdgeToLoop> edgeToLoops(numEdges);
			memset(edgeToLoops.data(), 0, edgeToLoops.size() * sizeof(edgeToLoops[0]));

			const auto loops_data_step = _mesh.get_loops_data_step<MLoop>();
			const auto pLoops = _mesh.get_loops<char*>();
			blrna::MLoop mloopObjectWork(PointerRNA{ (ID*)_mesh.data(), _mesh.loops_item_type(), nullptr });

			float n[6];
			mloopObjectWork.SetMLoop(&pLoops[0]);
			mloopObjectWork.get_normal(n);
			if (n[0] == 0 && n[1] == 0 && n[2] == 0)
			{
				// 法線が設定されていなければ計算する
				_mesh.calc_normals_split();
			}

			for (int pi = 0; pi < numPolygons; pi++)
			{
				const auto loop = polyLoops[pi];
				for (int iNext = loop.first, iCurr = loop.second - 1, iPrev = loop.second - 2; iNext < loop.second; iPrev = iCurr, iCurr = iNext++)
				{
					auto loop_e = cornerEdges[iCurr];
					auto& flag = edgeFlags[loop_e];
					if (flag & RenderApp::FaceFlag_SmoothingBound0_1)
					{
						continue;
					}

					auto& etol = edgeToLoops[loop_e];
					const auto& edge = pEdges[loop_e];
					if (etol.isNull())
					{
						etol.Set(iPrev, iCurr, iNext, cornerVerts, edge);
					}
					else
					{
						etolTmp.Set(iPrev, iCurr, iNext, cornerVerts, edge);

						etol.GetNormal(n, 0, pLoops, mloopObjectWork, loops_data_step);
						etolTmp.GetNormal(n + 3, 0, pLoops, mloopObjectWork, loops_data_step);
						if (n[0] != n[3] || n[1] != n[4] || n[2] != n[5])
						{
							flag |= RenderApp::FaceFlag_SmoothingBound0_1;
						}
						else
						{
							etol.GetNormal(n, 1, pLoops, mloopObjectWork, loops_data_step);
							etolTmp.GetNormal(n + 3, 1, pLoops, mloopObjectWork, loops_data_step);
							if (n[0] != n[3] || n[1] != n[4] || n[2] != n[5])
							{
								flag |= RenderApp::FaceFlag_SmoothingBound0_1;
							}
						}
					}
				}
			}
		}
	}

	std::vector<void*> CurveData::ConvertPyObjectToPtr(const std::vector<py::object>& materials)
	{
		std::vector<void*> ret;
		ret.reserve(materials.size());

		for (auto obj : materials)
		{
			ret.emplace_back(blrna::ConvertToRNAData(obj));
		}

		return ret;
	}
}