#pragma once
#include <vector>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include "Nodes.h"

namespace RenderApp
{
	static constexpr int FaceFlag_DoubleSided = (1 << 0);
	static constexpr int FaceFlag_Wire0_1 = (1 << 1);
	static constexpr int FaceFlag_Wire1_2 = (1 << 2);
	static constexpr int FaceFlag_Wire2_0 = (1 << 3);
	static constexpr int FaceFlag_SelectedEdge0_1 = (1 << 4);
	static constexpr int FaceFlag_SelectedEdge1_2 = (1 << 5);
	static constexpr int FaceFlag_SelectedEdge2_0 = (1 << 6);
	static constexpr int FaceFlag_SmoothingBound0_1 = (1 << 7);
	static constexpr int FaceFlag_SmoothingBound1_2 = (1 << 8);
	static constexpr int FaceFlag_SmoothingBound2_0 = (1 << 9);

#pragma pack(4)
	struct DataHeader
	{
		const size_t headerBytes = sizeof(DataHeader);
		const int version = 6;

		size_t dataBytes = 0;
		size_t meshDataBytes = 0;
		size_t dataBytesStart = 0;
		int renderType = 0; // reserved
	};

	struct Vector2
	{
		float x;
		float y;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(x), CEREAL_NVP(y));
		}
	};

	struct Vector3
	{
		float x;
		float y;
		float z;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z));
		}
	};

	struct ColorFloat
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct Matrix4x4
	{
		float m00;
		float m10;
		float m20;
		float m30;
		float m01;
		float m11;
		float m21;
		float m31;
		float m02;
		float m12;
		float m22;
		float m32;
		float m03;
		float m13;
		float m23;
		float m33;

		void Set(const float* ptr) { memcpy(&m00, ptr, sizeof(float) * 16); }

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(m00),
				CEREAL_NVP(m10),
				CEREAL_NVP(m20),
				CEREAL_NVP(m30),
				CEREAL_NVP(m01),
				CEREAL_NVP(m11),
				CEREAL_NVP(m21),
				CEREAL_NVP(m31),
				CEREAL_NVP(m02),
				CEREAL_NVP(m12),
				CEREAL_NVP(m22),
				CEREAL_NVP(m32),
				CEREAL_NVP(m03),
				CEREAL_NVP(m13),
				CEREAL_NVP(m23),
				CEREAL_NVP(m33)
			);
		}
	};

	//struct MeshData
	//{
	//	std::vector<Vector3> vertices;
	//	std::vector<Vector2> uvs;
	//	std::vector<int> triangles;
	//};

	struct MeshDataInfo
	{
		int vertexCount = 0;
		std::vector<int> subMeshPrimitiveCounts;
		std::vector<int> subMeshMaterialIds;
		bool triangleFlagsEnabled = true;
		std::vector<int> uvChannels;
		int objectType = 0;	// 0 : triangle , 1 : line
		std::vector<int> vertColorChannels;
		std::vector<int> faceCornerColorChannels;
		std::vector<int> uvChannelIDs;
		std::vector<int> colorChannelIDs;

		int IndexCount(int primitiveCount) const
		{
			switch (objectType)
			{
			case 0: return primitiveCount * 3;
			case 1: return primitiveCount * 2;
			default: return  0;
			}
		}
		int IndexCountAll() const
		{
			int ret = 0;
			for (auto primitiveCount : subMeshPrimitiveCounts)
			{
				ret += IndexCount(primitiveCount);
			}
			return ret;
		}
		int PrimitiveCountAll() const
		{
			int ret = 0;
			for (auto primitiveCount : subMeshPrimitiveCounts)
			{
				ret += primitiveCount;
			}
			return ret;
		}

		size_t VerticesDataSize() const { return sizeof(Vector3) * vertexCount; }
		size_t IndicesDataSize() const { return sizeof(int) * IndexCountAll(); }
		size_t TriangleFlagsDataSize() const { return triangleFlagsEnabled ? sizeof(int) * PrimitiveCountAll() : 0; }
		size_t UVsDataSize() const { return sizeof(Vector2) * IndexCountAll() * uvChannels.size(); }
		size_t ColorDataSize() const { return sizeof(ColorFloat) * (vertexCount * vertColorChannels.size() + IndexCountAll() * faceCornerColorChannels.size()); }

		Vector3* VerticesData(void* ptr) const { return reinterpret_cast<Vector3*>(VerticesDataImpl(ptr)); }
		int* IndicesData(void* ptr) const { return reinterpret_cast<int*>(IndicesDataImpl(ptr)); }
		int* TriangleFlagsData(void* ptr) const { return reinterpret_cast<int*>(TriangleFlagsDataImpl(ptr)); }
		Vector2* UVsData(void* ptr) const { return reinterpret_cast<Vector2*>(UVsDataImpl(ptr)); }
		ColorFloat* ColorData(void* ptr) const { return reinterpret_cast<ColorFloat*>(ColorDataImpl(ptr)); }

		size_t MeshDataSize() const
		{
			int dummy = 0;
			return ColorDataImpl(&dummy) - VerticesDataImpl(&dummy) + ColorDataSize();
		}

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(vertexCount),
				CEREAL_NVP(subMeshPrimitiveCounts),
				CEREAL_NVP(subMeshMaterialIds),
				CEREAL_NVP(triangleFlagsEnabled),
				CEREAL_NVP(uvChannels),
				CEREAL_NVP(objectType)
			);
			if (1 <= version) {
				archive(
					CEREAL_NVP(vertColorChannels),
					CEREAL_NVP(faceCornerColorChannels),
					CEREAL_NVP(uvChannelIDs),
					CEREAL_NVP(colorChannelIDs)
				);
			}
		}

	private:
		char* VerticesDataImpl(void* ptr) const { return static_cast<char*>(ptr); }
		char* IndicesDataImpl(void* ptr) const { return VerticesDataImpl(ptr) + VerticesDataSize(); }
		char* TriangleFlagsDataImpl(void* ptr) const { return IndicesDataImpl(ptr) + IndicesDataSize(); }
		char* UVsDataImpl(void* ptr) const { return TriangleFlagsDataImpl(ptr) + TriangleFlagsDataSize(); }
		char* ColorDataImpl(void* ptr) const { return UVsDataImpl(ptr) + UVsDataSize(); }

	};

	struct RenderInstance
	{
		int instanceId = 0;
		Matrix4x4 localToWolrdMatrix;
		int meshDataIndex = 0;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(instanceId),
				CEREAL_NVP(localToWolrdMatrix),
				CEREAL_NVP(meshDataIndex)
			);
		}
	};

	struct Camera
	{
		Matrix4x4 localToWolrdMatrix;
		Matrix4x4 projectionMatrix;
		Matrix4x4 viewportMatrix;
		float nearClip = 0.0f;
		float farClip = 0.0f;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(localToWolrdMatrix),
				CEREAL_NVP(projectionMatrix),
				CEREAL_NVP(viewportMatrix),
				CEREAL_NVP(nearClip),
				CEREAL_NVP(farClip)
			);
		}
	};

	struct RenderInformation
	{
		int width;
		int height;
		int pixelBufferFormat = 0; // 0:RGBA32, 1:RGBA64, 2:RGBAFloat
		bool flipY = false;
		float lineScale = 1.0f;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(width),
				CEREAL_NVP(height),
				CEREAL_NVP(pixelBufferFormat),
				CEREAL_NVP(flipY)
			);
			if (1 <= version) {
				archive(
					CEREAL_NVP(lineScale)
				);
			}
		}
	};

	struct Data
	{
		RenderInformation renderInformation;
		Camera camera;
		std::vector<RenderInstance> renderInstances;
		std::vector<MeshDataInfo> meshDataInformations;
		std::vector<std::shared_ptr<Nodes::LineNodeToExport>> lineNodes;
		std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>> lineFunctions;
		std::vector<std::vector<int>> groups;
		std::wstring taskName;
		std::vector<std::shared_ptr<Nodes::LineRenderElementToExport>> lineRenderElements;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(renderInformation),
				CEREAL_NVP(camera),
				CEREAL_NVP(renderInstances),
				CEREAL_NVP(meshDataInformations),
				CEREAL_NVP(lineNodes),
				CEREAL_NVP(lineFunctions),
				CEREAL_NVP(groups)
			);
			if (1 <= version) {
				archive(
					CEREAL_NVP(taskName)
				);
			}
			if (2 <= version) {
				archive(
					CEREAL_NVP(lineRenderElements)
				);
			}
		}
	};

#pragma pack()
}

CEREAL_CLASS_VERSION(RenderApp::MeshDataInfo, 1);
CEREAL_CLASS_VERSION(RenderApp::RenderInformation, 1);
CEREAL_CLASS_VERSION(RenderApp::Data, 2);
