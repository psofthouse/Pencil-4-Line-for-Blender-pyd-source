#include <algorithm>
#include <sstream>

#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>

#include "blrnaImage.h"
#include "blrnaMesh.h"
#include "blrnaMLoop.h"
#include "blrnaMEdge.h"
#include "blrnaMaterial.h"
#include "intermCamera.h"
#include "intermRenderInstance.h"
#include "intermContext.h"
#include "intermMeshDataAccessor.h"
#include "intermObjectIDMapper.h"
#include "intermImageMapper.h"
#include "intermNameIDMapper.h"
#include "intermNodeParamsFixer.h"
#include "intermDataHash.h"
#include "intermDrawOptions.h"
#include "RenderAppData.h"
#include "RenderAppSession.h"
#include "DNA_meshdata_types.h"

namespace interm
{
	constexpr int RenderAppPixelFormat_RGBA32 = 0;
	constexpr int RenderAppPixelFormat_Float = 2;

	RenderAppRet Context::DrawForViewport(int width, int height,
		Camera camera,
		const std::vector<py::object>& renderInstances,
		const py::object materialOverride,
		const std::vector<std::pair<py::object, py::object>>& curveData,
		const std::vector<std::shared_ptr<Nodes::LineNodeToExport>>& lineNodesSrc,
		const std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>>& lineFunctions,
		const std::vector<std::vector<py::object>>& groups)
	{
		if (!_viewport_image_buffer || !_data_hash)
		{
			_viewport_image_buffer = std::make_shared<std::vector<float>>();
			_data_hash = std::make_shared<DataHash>();
		}
		CleanupFrame();
		std::array<float, 2> viewportScale = { (float)width , (float)height };

		return DrawImpl(width, height, blrna::Image(py::object()), camera, viewportScale,
			renderInstances, materialOverride, curveData, lineNodesSrc, lineFunctions,
			std::vector<std::shared_ptr<Nodes::LineRenderElementToExport>>(),
			std::vector<std::shared_ptr<Nodes::VectorOutputToExport>>(),
			groups);
	}

	RenderAppRet Context::Draw(blrna::Image image,
		Camera camera,
		const std::vector<py::object>& renderInstances,
		const py::object materialOverride,
		const std::vector<std::pair<py::object, py::object>>& curveData,
		const std::vector<std::shared_ptr<Nodes::LineNodeToExport>>& lineNodesSrc,
		const std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>>& lineFunctions,
		const std::vector<std::shared_ptr<Nodes::LineRenderElementToExport>>& lineRenderElements,
		const std::vector<std::shared_ptr<Nodes::VectorOutputToExport>>& vectorOutputs,
		const std::vector<std::vector<py::object>>& groups)
	{
		int w = 0, h = 0;
		if (image)
		{
			w = image.Width();
			h = image.Height();
		}
		for (auto element : lineRenderElements)
		{
			auto i = blrna::Image(element->Image);
			if ((w > 0 && w != i.Width()) || (h > 0 && h != i.Height()))
			{
				w = 0;
				h = 0;
				break;
			}
			w = i.Width();
			h = i.Height();
		}
		if (w == 0 || h == 0)
		{
			return RenderAppRet::Error_InvalidParams;
		}

		std::array<float, 2> viewportScale;
		switch (camera.GetSensorFit())
		{
		default:	viewportScale = { (float)std::max(w, h), (float)std::max(w, h) }; break;
		case 1:		viewportScale = { float(w), float(w) }; break;
		case 2:		viewportScale = { float(h), float(h) }; break;
		}

		return DrawImpl(w, h, image, camera, viewportScale,
			renderInstances, materialOverride, curveData, lineNodesSrc, lineFunctions, lineRenderElements, vectorOutputs, groups);
	}

	RenderAppRet Context::DrawImpl(int w, int h,
		blrna::Image image,
		Camera camera,
		const std::array<float, 2>& viewportScale,
		const std::vector<py::object>& renderInstances,
		const py::object materialOverride,
		const std::vector<std::pair<py::object, py::object>>& curveData,
		const std::vector<std::shared_ptr<Nodes::LineNodeToExport>>& lineNodesSrc,
		const std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>>& lineFunctions,
		const std::vector<std::shared_ptr<Nodes::LineRenderElementToExport>>& lineRenderElements,
		const std::vector<std::shared_ptr<Nodes::VectorOutputToExport>>& vectorOutputs,
		const std::vector<std::vector<py::object>>& groups)
	{
		auto renderSession = RenderApp::Session::Create(renderAppPath, drawOptions ? drawOptions->timeout : 0);
		if (!renderSession)
		{
			return RenderAppRet::Error_Unknown;
		}
		if (!renderSession->IsReady())
		{
			return renderSession->GetLastRenderAppRet();
		}
		if (w <= 0 || h <= 0)
		{
			return RenderAppRet::Error_InvalidParams;
		}

		//
		RenderApp::DataHeader header;
		RenderApp::Data renderAppData;
		renderAppData.taskName = taskName;
		renderAppData.renderInformation.width = w;
		renderAppData.renderInformation.height = h;
		renderAppData.renderInformation.pixelBufferFormat = RenderAppPixelFormat_Float;
		renderAppData.renderInformation.flipY = true;
		renderAppData.renderInformation.lineScale = drawOptions ? drawOptions->line_scale : 1.0f;
		const size_t bytesPerPixel = 4 * sizeof(float);

		// カメラ設定
		renderAppData.camera.nearClip = camera.GetNearClip();
		renderAppData.camera.farClip = camera.GetFarClip();
		renderAppData.camera.localToWolrdMatrix.Set(camera.GetMatrixWorld());
		renderAppData.camera.projectionMatrix.Set(camera.GetProjection());
		renderAppData.camera.viewportMatrix = {
			0.5f * viewportScale[0], 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f * viewportScale[1], 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		// レンダーエレメント設定
		ImageMapper imageMapperForRenderElement(sizeof(header), renderSession->GetDataGranularity());
		for (auto element : lineRenderElements)
		{
			auto i = blrna::Image(element->Image);

			element->pixelBufferFormat = i.get_is_float() ? RenderAppPixelFormat_Float : RenderAppPixelFormat_RGBA32;

			auto bytesPerPixel = element->pixelBufferFormat ? 32 : 4;
			element->ptr_offset = imageMapperForRenderElement.GetPtrOffset(element->Image, bytesPerPixel);

			element->depthDrawModeEnabled = element->ElementType == Nodes::LineRenderElementToExport::RenderElementType::Depth;
			element->backgroundColorR = element->BackgroundColorRGBA[0];
			element->backgroundColorG = element->BackgroundColorRGBA[1];
			element->backgroundColorB = element->BackgroundColorRGBA[2];
			element->backgroundColorA = element->BackgroundColorRGBA[3];
			element->isDrawLineSetId1 = element->LinesetIDs[0];
			element->isDrawLineSetId2 = element->LinesetIDs[1];
			element->isDrawLineSetId3 = element->LinesetIDs[2];
			element->isDrawLineSetId4 = element->LinesetIDs[3];
			element->isDrawLineSetId5 = element->LinesetIDs[4];
			element->isDrawLineSetId6 = element->LinesetIDs[5];
			element->isDrawLineSetId7 = element->LinesetIDs[6];
			element->isDrawLineSetId8 = element->LinesetIDs[7];
		}

		ImageMapper imageMapperForTexture(lineRenderElements.empty() ? sizeof(header) : imageMapperForRenderElement.GetNextPtrOffset(), 1);

		// ベクトルファイル出力設定
		for (auto vectorOutput : vectorOutputs)
		{
			vectorOutput->isDrawLineSetId1 = vectorOutput->LinesetIDs[0];
			vectorOutput->isDrawLineSetId2 = vectorOutput->LinesetIDs[1];
			vectorOutput->isDrawLineSetId3 = vectorOutput->LinesetIDs[2];
			vectorOutput->isDrawLineSetId4 = vectorOutput->LinesetIDs[3];
			vectorOutput->isDrawLineSetId5 = vectorOutput->LinesetIDs[4];
			vectorOutput->isDrawLineSetId6 = vectorOutput->LinesetIDs[5];
			vectorOutput->isDrawLineSetId7 = vectorOutput->LinesetIDs[6];
			vectorOutput->isDrawLineSetId8 = vectorOutput->LinesetIDs[7];
		}

		//
		std::unordered_map<void*, const CurveData&> curveDataMap;
		for (auto& pair : curveData)
		{
			auto pMeshData = blrna::ConvertToRNAData(pair.first);
			auto itr = curveDataMap.find(pMeshData);
			if (itr == curveDataMap.end())
			{
				curveDataMap.emplace(pMeshData, *pair.second.cast<CurveData*>());
			}
		}

		std::unordered_map<void*, const std::vector<py::object>&> meshColorAttributesMap;
		if (mesh_color_attributes_on)
		{
			for (auto& pair : mesh_color_attributes)
			{
				meshColorAttributesMap.emplace(blrna::ConvertToRNAData(pair.first), pair.second);
			}
		}

		// レンダーインスタンス設定
		renderAppData.renderInstances.reserve(renderInstances.size());
		std::vector<int> holdout_instace_indices;
		holdout_instace_indices.reserve(renderInstances.size());
		std::vector<std::shared_ptr<MeshDataAccessor>> meshDataAccessors;
		{
			blrna::Material mtlOverride(materialOverride);
			std::unordered_map<void*, int> meshDataToIndex;
			int instanceId = 0;
			for (auto& renderInstance : renderInstances)
			{
				const auto& src = *renderInstance.cast<RenderInstance*>();

				renderAppData.renderInstances.emplace_back(RenderApp::RenderInstance());
				auto& dst = renderAppData.renderInstances.back();
				dst.localToWolrdMatrix.Set(src.GetMatrixWorld());

				dst.instanceId = instanceId++;

				if (src.IsHoldout())
				{
					holdout_instace_indices.emplace_back(dst.instanceId);
				}

				// メッシュデータへの参照設定
				const auto& srcMesh = blrna::Mesh(src.Mesh());
				auto itr = meshDataToIndex.find(srcMesh.data());
				if (itr == meshDataToIndex.end())
				{
					// MeshDataAccessor新規生成
					auto meshColorAttributesItr = mesh_color_attributes_on ? meshColorAttributesMap.find(srcMesh.data()) : meshColorAttributesMap.end();
					const auto& mesh_color_attributes = mesh_color_attributes_on ?
						(meshColorAttributesItr != meshColorAttributesMap.end() ? meshColorAttributesItr->second : MeshDataAccessor::color_attributes_default) :
						MeshDataAccessor::color_attributes_none;

					auto curveDataItr = curveDataMap.find(srcMesh.data());
					meshDataAccessors.emplace_back(curveDataItr == curveDataMap.end() ?
						std::make_shared<MeshDataAccessor>(srcMesh, *this, mtlOverride, mesh_color_attributes) :
						std::make_shared<MeshDataAccessorForCurve>(srcMesh, *this, mtlOverride, mesh_color_attributes, curveDataItr->second));
					dst.meshDataIndex = (int)meshDataAccessors.size() - 1;
					meshDataToIndex.emplace(srcMesh.data(), dst.meshDataIndex);
				}
				else
				{
					// 生成済みのMeshDataAccessorへの参照を設定
					dst.meshDataIndex = itr->second;
				}
			}
		}

		// ノード設定
		std::vector<std::shared_ptr<Nodes::LineNodeToExport>> lineNodes;
		lineNodes.reserve(lineNodesSrc.size());

		{
			ObjectIDMapper objectIDMapper(renderAppData.renderInstances, renderInstances);
			NameIDMapper nameIDMapper;
			NodeParamsFixer fixer(_instanceIDMap, objectIDMapper, imageMapperForTexture, nameIDMapper);
			auto linesize_relative_target_width = (drawOptions && drawOptions->linesize_relative_target_width > 0) ? drawOptions->linesize_relative_target_width : w;
			auto linesize_relative_target_height = (drawOptions && drawOptions->linesize_relative_target_height > 0) ? drawOptions->linesize_relative_target_height : h;

			for (auto& lineNodeSrc : lineNodesSrc)
			{
				if (lineNodeSrc && lineNodeSrc->Active)
				{
					if (lineNodeSrc->LineSizeTypeValue != Nodes::LineSizeType::Absolute)
					{
						lineNodeSrc->LineSizeTypeValue = Nodes::LineSizeType::Absolute;
						if (camera.GetSensorFit() == 1 ||
							(camera.GetSensorFit() == 0 && linesize_relative_target_width >= linesize_relative_target_height))
						{
							lineNodeSrc->ScaleEx *= linesize_relative_target_width / 640.f;
						}
						else
						{
							lineNodeSrc->ScaleEx *= linesize_relative_target_height / 480.f;
						}
					}

					fixer.FixNode(lineNodeSrc);
					lineNodes.emplace_back(lineNodeSrc);
				}
			}

			for (auto& lineFunc : lineFunctions)
			{
				fixer.FixNode(lineFunc);
			}

			// Holdout設定
			if (!holdout_instace_indices.empty())
			{
				for (auto linenode : lineNodes)
				{
					auto dummyLineSetNode = std::make_shared<Nodes::LineSetNodeToExport>();
					dummyLineSetNode->ObjectIds = holdout_instace_indices;
					linenode->LineSetNodesToExport.insert(linenode->LineSetNodesToExport.begin(), dummyLineSetNode);
				}
			}

			// メッシュが使用するマップチャンネル情報を設定
			fixer.ForMapChannelsPerLinesets([&](std::shared_ptr<Nodes::LineSetNodeToExport> lineset,
				const std::unordered_set<int>& uvChannels, const std::unordered_set<int>& colorChannels,
				const std::unordered_set<std::string>& uvNames, const std::unordered_set<std::string>& colorNames)
			{
				std::unordered_set<int> objects(lineset->ObjectIds.begin(), lineset->ObjectIds.end());
				std::unordered_set<int> materials(lineset->MaterialIds.begin(), lineset->MaterialIds.end());

				for (const auto& renderInstance : renderAppData.renderInstances)
				{
					bool hit = false;
					auto meshData = meshDataAccessors[renderInstance.meshDataIndex];

					if (objects.find(renderInstance.instanceId) != objects.end())
					{
						hit = true;
					}
					else
					{
						for (const auto& materialID : meshData->info.subMeshMaterialIds)
						{
							if (materials.find(materialID) != materials.end())
							{
								hit = true;
								break;
							}
						}
					}

					if (hit)
					{
						for (const auto& channel : uvChannels)
						{
							meshData->ActivateUVChannel(channel);
						}
						for (const auto& channel : colorChannels)
						{
							meshData->ActivateColorChannel(channel);
						}
						for (const auto& name : uvNames)
						{
							meshData->ActivateUVChannel(name, nameIDMapper);
						}
						for (const auto& name : colorNames)
						{
							meshData->ActivateColorChannel(name, nameIDMapper);
						}
					}
				}
			});

			// グループ設定
			for (auto& groupSrc : groups)
			{
				auto& groupDst = renderAppData.groups.emplace_back();
				objectIDMapper.ExtractIDs(groupDst, groupSrc);
			}
		}

		renderAppData.lineNodes = lineNodes;
		renderAppData.lineFunctions = lineFunctions;
		renderAppData.lineRenderElements = lineRenderElements;
		renderAppData.vectorOutputs = vectorOutputs;
		renderAppData.platform = platform;

		// メッシュデータ情報を送信用データに書き込む
		renderAppData.meshDataInformations.reserve(meshDataAccessors.size());
		for (auto& meshDataAccessor : meshDataAccessors)
		{
			renderAppData.meshDataInformations.emplace_back(meshDataAccessor->info);
		}

		// レンダリングに必要なデータサイズを計算する
		std::string renderAppDataBinary;
		{
			std::stringstream stream;
			cereal::BinaryOutputArchive archive(stream);
			archive(CEREAL_NVP(renderAppData));
			renderAppDataBinary = stream.str();
		}

		header.dataBytes = renderAppDataBinary.size();
		header.dataBytesStart = imageMapperForTexture.GetNextPtrOffset();

		header.meshDataBytes = 0;
		for (auto& meshDataInfo : renderAppData.meshDataInformations)
		{
			header.meshDataBytes += meshDataInfo.MeshDataSize();
		}

		auto allBufferSize = header.dataBytesStart + std::max(header.dataBytes + header.meshDataBytes, w * h * bytesPerPixel);

		// データを書き込むバッファの確保
		if (!renderSession->RequestData(allBufferSize))
		{
			return renderSession->GetLastRenderAppRet();
		}

		// 確保したバッファへのデータ書き込み
		std::shared_ptr<DataHash> data_hash, data_hash_excluding_objects;
		if (_data_hash)
		{
			data_hash = std::make_shared<DataHash>();
		}

		auto writeDataFunc = [&](std::function<void(void*)> writeFunc, size_t offset, size_t bytes)
		{
			auto dataAccessor = renderSession->AccessData(offset, bytes, RenderApp::DataAccessor::DesiredAccess::Write);
			if (dataAccessor)
			{
				writeFunc(dataAccessor->ptr());
				if (data_hash)
				{
					data_hash->Record(offset, bytes, dataAccessor->ptr());
				}
				return true;
			}


			return false;
		};
		auto writeDataBuffer = [&](const void* src, size_t offset, size_t bytes)
		{
			return writeDataFunc([src, bytes](void* dst) {
				memcpy(dst, src, bytes);
			}, offset, bytes);
		};

		if (!writeDataBuffer(&header, 0, sizeof(header))) return RenderAppRet::Error_Unknown;

		size_t offset = header.dataBytesStart;
		if (!writeDataBuffer(renderAppDataBinary.data(), offset, renderAppDataBinary.size())) return RenderAppRet::Error_Unknown;
		offset += renderAppDataBinary.size();

		if (data_hash)
		{
			// 前回の描画完了時からオブジェクトの状態が変化していないと指定されている場合には、
			// オブジェクトのハッシュを比較することなくパラーメータの比較のみを行い、
			// 可能な限り早くリターンする
			data_hash_excluding_objects = std::make_shared<DataHash>(*data_hash);
			if (drawOptions && drawOptions->objects_cache_valid &&
				_data_hash_excluding_objects && *data_hash_excluding_objects == *_data_hash_excluding_objects)
			{
				return RenderAppRet::Success;
			}
		}

		for (auto meshDataAccessor : meshDataAccessors)
		{
			if (!writeDataFunc([&meshDataAccessor](void* dst) {
				meshDataAccessor->WriteDataToBuffer(static_cast<char*>(dst));
			}, offset, meshDataAccessor->info.MeshDataSize()))
			{
				return RenderAppRet::Error_Unknown;
			}
			offset += meshDataAccessor->info.MeshDataSize();
		}

		// テクスチャマップの書き込み
		{
			std::shared_ptr<RenderApp::DataAccessor> dataAccessor;
			if (!imageMapperForTexture.WriteImageData([&](size_t offset, size_t bytes) {
				dataAccessor = renderSession->AccessData(offset, bytes, RenderApp::DataAccessor::DesiredAccess::Write);
				return dataAccessor ? dataAccessor->ptr() : nullptr;
			}, [&](size_t offset, size_t bytes) {
				if (data_hash)
				{
					data_hash->Record(offset, bytes, dataAccessor->ptr());
				}
			}, _textureWorkBuffer))
			{
				return RenderAppRet::Error_Unknown;
			}
		}
		
		// レンダリングの実行
		if (data_hash && *data_hash == *_data_hash)
		{
			return RenderAppRet::Success;
		}

		_data_hash = nullptr;
		if (!renderSession->Render())
		{
			return renderSession->GetLastRenderAppRet();
		}

		// 得られたピクセルデータをImageに設定する
		if (image)
		{
			auto dataAccessor = renderSession->AccessData(header.dataBytesStart, w * h * bytesPerPixel, RenderApp::DataAccessor::DesiredAccess::Read);
			if (!dataAccessor)
			{
				return RenderAppRet::Error_Unknown;
			}
			image.set_pixels(dataAccessor->ptr<float*>());
			image.update();
		}
		for (auto element : lineRenderElements)
		{
			auto i = blrna::Image(element->Image);
			auto bytesPerPixel = element->pixelBufferFormat ? 32 : 4;
			auto dataAccessor = renderSession->AccessData(element->ptr_offset, w * h * bytesPerPixel, RenderApp::DataAccessor::DesiredAccess::Read);
			if (!dataAccessor)
			{
				return RenderAppRet::Error_Unknown;
			}

			if (bytesPerPixel == 32)
			{
				i.set_pixels(dataAccessor->ptr<float*>());
				i.update();
			}
			else
			{
				int numData = w * h * 4;
				_textureWorkBuffer.resize(numData);
				auto pSrc = dataAccessor->ptr<unsigned char*>();
				auto pDst = _textureWorkBuffer.data();
				constexpr auto coef = 1.0f / 255;
				for (int i = 0; i < numData; i++)
				{
					pDst[i] = coef * pSrc[i];
				}
				i.set_pixels(pDst);
				i.update();
			}
		}

		if (_viewport_image_buffer)
		{
			auto& dst = *_viewport_image_buffer;
			dst.resize(w * h * 4);
			auto dataAccessor = renderSession->AccessData(header.dataBytesStart, w * h * bytesPerPixel, RenderApp::DataAccessor::DesiredAccess::Read);
			if (!dataAccessor)
			{
				return RenderAppRet::Error_Unknown;
			}
			memcpy(dst.data(), dataAccessor->ptr<unsigned char*>(), dst.size() * sizeof(float));
		}

		_data_hash = data_hash;
		_data_hash_excluding_objects = data_hash_excluding_objects;
		return renderSession ? renderSession->GetLastRenderAppRet() : RenderAppRet::Success;
	}

	void Context::CleanupFrame()
	{
		_instanceIDMap.clear();
}

	void Context::CleanupAll()
	{
		CleanupFrame();
		_viewport_image_buffer = nullptr;
		_data_hash = nullptr;
		_data_hash_excluding_objects = nullptr;
	}

	int Context::PointerToInstanceID(void* p)
	{
		int ret = -1;
		if (p)
		{
			auto itr = _instanceIDMap.find(p);
			if (itr != _instanceIDMap.end())
			{
				ret = itr->second;
			}
			else
			{
				ret = (int)_instanceIDMap.size();
				_instanceIDMap.emplace(p, ret);
			}
		}

		return ret;
	}
}