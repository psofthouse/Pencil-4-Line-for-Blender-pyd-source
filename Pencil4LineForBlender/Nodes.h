#pragma once

#include <memory>
#include <vector>
#include <optional>
#ifdef PYBIND_INCLUDE
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#endif
#include "StructDefinitions.h"
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/array.hpp>

namespace Nodes
{
#ifdef PYBIND_INCLUDE
	void registerEnums(const pybind11::module_& m);

	enum class LineSizeType : int
	{
		Absolute = 0,
		Relative
	};
	enum class LoopDirectionType : int
	{
		Clockwise = 0,
		Anticlockwise
	};

	enum class TextureMapSourceType : int
	{
		Image = 0,
		ObjectColor
	};
	enum class TextureMapUVSource : int
	{
		Screen = 0,
		ObjectUV
	};
	enum class TextureMapSelectionMode : int
	{
		Index = 0,
		Name
	};
#endif
	struct LineNodeToExport;
	struct LineSetNodeToExport;
	struct BrushSettingsNodeToExport;
	struct BrushDetailNodeToExport;
	struct ReductionSettingsNodeToExport;
	struct TextureMapNodeToExport;

	struct LineNodeToExport : public Pcl4NativeDll::LineNode
	{
#ifdef PYBIND_INCLUDE
		LineNodeToExport()
		{
			ScaleEx = 1.0f;
		}
		static void registerClass(const pybind11::module_& m);
		Nodes::LineSizeType LineSizeTypeValue; // -> LineSizeType
#endif
		std::vector<std::shared_ptr<LineSetNodeToExport>> LineSetNodesToExport;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(Active),
				CEREAL_NVP(RenderPriority),
				CEREAL_NVP(LineSetNodesToExport),
				CEREAL_NVP(LineSizeType),
				CEREAL_NVP(OutputToRenderElementsOnly),
				CEREAL_NVP(LineOversampling),
				CEREAL_NVP(LineAntialiasing),
				CEREAL_NVP(LineOffscreenDistance),
				CEREAL_NVP(LineRandomSeed)
			);

			if (1 <= version) {
				archive(
					CEREAL_NVP(ScaleEx)
				);
			}
			else
			{
				ScaleEx = 1.0f;
			}
		}
	};

	struct LineSetNodeToExport : public Pcl4NativeDll::LineSetNode
	{
#ifdef PYBIND_INCLUDE
		static void registerClass(const pybind11::module_& m);

		std::vector<pybind11::object> ObjectsToExport;
		std::vector<pybind11::object> MaterialsToExport;
#endif
		std::vector<int> ObjectIds;
		std::vector<int> MaterialIds;

		std::shared_ptr<BrushSettingsNodeToExport> VBrushSettingsToExport;
		std::shared_ptr<BrushSettingsNodeToExport> VOutlineToExport;
		std::shared_ptr<BrushSettingsNodeToExport> VObjectToExport;
		std::shared_ptr<BrushSettingsNodeToExport> VIntersectionToExport;
		std::shared_ptr<BrushSettingsNodeToExport> VSmoothToExport;
		std::shared_ptr<BrushSettingsNodeToExport> VMaterialToExport;
		std::shared_ptr<BrushSettingsNodeToExport> VNormalAngleToExport;
		std::shared_ptr<BrushSettingsNodeToExport> VWireframeToExport;
		std::shared_ptr<BrushSettingsNodeToExport> VSelectedToExport;
		std::shared_ptr<ReductionSettingsNodeToExport> VSizeReductionToExport;
		std::shared_ptr<ReductionSettingsNodeToExport> VAlphaReductionToExport;

		std::shared_ptr<BrushSettingsNodeToExport> HBrushSettingsToExport;
		std::shared_ptr<BrushSettingsNodeToExport> HOutlineToExport;
		std::shared_ptr<BrushSettingsNodeToExport> HObjectToExport;
		std::shared_ptr<BrushSettingsNodeToExport> HIntersectionToExport;
		std::shared_ptr<BrushSettingsNodeToExport> HSmoothToExport;
		std::shared_ptr<BrushSettingsNodeToExport> HMaterialToExport;
		std::shared_ptr<BrushSettingsNodeToExport> HNormalAngleToExport;
		std::shared_ptr<BrushSettingsNodeToExport> HWireframeToExport;
		std::shared_ptr<BrushSettingsNodeToExport> HSelectedToExport;
		std::shared_ptr<ReductionSettingsNodeToExport> HSizeReductionToExport;
		std::shared_ptr<ReductionSettingsNodeToExport> HAlphaReductionToExport;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(On),
				CEREAL_NVP(Id),
				CEREAL_NVP(ObjectIds),
				CEREAL_NVP(MaterialIds),
				CEREAL_NVP(WeldsEdges),
				CEREAL_NVP(MaskHiddenLines),

				CEREAL_NVP(VBrushSettingsToExport),
				CEREAL_NVP(VOutlineOn),
				CEREAL_NVP(VOutlineOpenEdge),
				CEREAL_NVP(VOutlineMergeGroups),
				CEREAL_NVP(VOutlineSpecificOn),
				CEREAL_NVP(VOutlineToExport),
				CEREAL_NVP(VObjectOn),
				CEREAL_NVP(VObjectOpenEdge),
				CEREAL_NVP(VObjectSpecificOn),
				CEREAL_NVP(VObjectToExport),
				CEREAL_NVP(VIntersectionOn),
				CEREAL_NVP(VIntersectionSelf),
				CEREAL_NVP(VIntersectionSpecificOn),
				CEREAL_NVP(VIntersectionToExport),
				CEREAL_NVP(VSmoothOn),
				CEREAL_NVP(VSmoothSpecificOn),
				CEREAL_NVP(VSmoothToExport),
				CEREAL_NVP(VMaterialOn),
				CEREAL_NVP(VMaterialSpecificOn),
				CEREAL_NVP(VMaterialToExport),
				CEREAL_NVP(VNormalAngleOn),
				CEREAL_NVP(VNormalAngleSpecificOn),
				CEREAL_NVP(VNormalAngleToExport),
				CEREAL_NVP(VNormalAngleMin),
				CEREAL_NVP(VNormalAngleMax),
				CEREAL_NVP(VWireframeOn),
				CEREAL_NVP(VWireframeSpecificOn),
				CEREAL_NVP(VWireframeToExport),
				CEREAL_NVP(VSelectedOn),
				CEREAL_NVP(VSelectedSpecificOn),
				CEREAL_NVP(VSelectedToExport),
				CEREAL_NVP(VSizeReductionOn),
				CEREAL_NVP(VSizeReductionToExport),
				CEREAL_NVP(VAlphaReductionOn),
				CEREAL_NVP(VAlphaReductionToExport),

				CEREAL_NVP(HBrushSettingsToExport),
				CEREAL_NVP(HOutlineOn),
				CEREAL_NVP(HOutlineMergeGroups),
				CEREAL_NVP(HOutlineSpecificOn),
				CEREAL_NVP(HOutlineToExport),
				CEREAL_NVP(HOutlineOpenEdge),
				CEREAL_NVP(HObjectOn),
				CEREAL_NVP(HObjectOpenEdge),
				CEREAL_NVP(HObjectSpecificOn),
				CEREAL_NVP(HObjectToExport),
				CEREAL_NVP(HIntersectionOn),
				CEREAL_NVP(HIntersectionSelf),
				CEREAL_NVP(HIntersectionSpecificOn),
				CEREAL_NVP(HIntersectionToExport),
				CEREAL_NVP(HSmoothOn),
				CEREAL_NVP(HSmoothSpecificOn),
				CEREAL_NVP(HSmoothToExport),
				CEREAL_NVP(HMaterialOn),
				CEREAL_NVP(HMaterialSpecificOn),
				CEREAL_NVP(HMaterialToExport),
				CEREAL_NVP(HNormalAngleOn),
				CEREAL_NVP(HNormalAngleSpecificOn),
				CEREAL_NVP(HNormalAngleToExport),
				CEREAL_NVP(HNormalAngleMin),
				CEREAL_NVP(HNormalAngleMax),
				CEREAL_NVP(HWireframeOn),
				CEREAL_NVP(HWireframeSpecificOn),
				CEREAL_NVP(HWireframeToExport),
				CEREAL_NVP(HSelectedOn),
				CEREAL_NVP(HSelectedSpecificOn),
				CEREAL_NVP(HSelectedToExport),
				CEREAL_NVP(HSizeReductionOn),
				CEREAL_NVP(HSizeReductionToExport),
				CEREAL_NVP(HAlphaReductionOn),
				CEREAL_NVP(HAlphaReductionToExport)

			);
		}
	};

	struct BrushSettingsNodeToExport : public Pcl4NativeDll::BrushSettingsNode
	{
#ifdef PYBIND_INCLUDE
		static void registerClass(const pybind11::module_& m);
		std::array<float, 3> ColorRGB; // ColorR, G, B
		bool ColorMapOn;
		bool SizeMapOn;
#endif
		std::shared_ptr<BrushDetailNodeToExport> BrushDetailNodeToExport;
		std::shared_ptr<TextureMapNodeToExport> ColorMapToExport;
		std::shared_ptr<TextureMapNodeToExport> SizeMapToExport;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(BrushDetailNodeToExport),
				CEREAL_NVP(BlendAmount),
				CEREAL_NVP(ColorR),
				CEREAL_NVP(ColorG),
				CEREAL_NVP(ColorB),
				CEREAL_NVP(ColorMapToExport),
				CEREAL_NVP(ColorMapOpacity),
				CEREAL_NVP(Size),
				CEREAL_NVP(SizeMapToExport),
				CEREAL_NVP(SizeMapAmount)
			);
		}
	};

	struct BrushDetailNodeToExport : public Pcl4NativeDll::BrushDetailNode
	{
#ifdef PYBIND_INCLUDE
		static void registerClass(const pybind11::module_& m);
		bool BrushMapOn;
		bool DistortionMapOn;
		Nodes::LoopDirectionType LoopDirectionTypeValue; // -> LoopDirectionType
		Pcl4NativeDll::ColorSpace ColorSpaceTypeValue; // -> ColorSpaceType
#endif
		std::shared_ptr<TextureMapNodeToExport> BrushMapToExport;
		std::shared_ptr<TextureMapNodeToExport> DistortionMapToExport;
		std::array<float, 256> SizeReductionCurveValues; // -> SizeReductionCurve
		std::array<float, 256> AlphaReductionCurveValues; // -> AlphaReductionCurve

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(BrushType),
				CEREAL_NVP(BrushMapToExport),
				CEREAL_NVP(BrushMapOpacity),
				CEREAL_NVP(Stretch),
				CEREAL_NVP(StretchRandom),
				CEREAL_NVP(Angle),
				CEREAL_NVP(AngleRandom),
				CEREAL_NVP(Groove),
				CEREAL_NVP(GrooveNumber),
				CEREAL_NVP(Size),
				CEREAL_NVP(SizeRandom),
				CEREAL_NVP(AntiAliasing),
				CEREAL_NVP(HorizontalSpace),
				CEREAL_NVP(HorizontalSpaceRandom),
				CEREAL_NVP(VerticalSpace),
				CEREAL_NVP(VerticalSpaceRandom),
				CEREAL_NVP(ReductionStart),
				CEREAL_NVP(ReductionEnd),

				CEREAL_NVP(StrokeType),
				CEREAL_NVP(LineType),
				CEREAL_NVP(Length),
				CEREAL_NVP(LengthRandom),
				CEREAL_NVP(Space),
				CEREAL_NVP(SpaceRandom),
				CEREAL_NVP(StrokeSizeRandom),
				CEREAL_NVP(Extend),
				CEREAL_NVP(ExtendRandom),
				CEREAL_NVP(LineCopy),
				CEREAL_NVP(LineCopyRandom),
				CEREAL_NVP(NormalOffset),
				CEREAL_NVP(NormalOffsetRandom),
				CEREAL_NVP(XOffset),
				CEREAL_NVP(XOffsetRandom),
				CEREAL_NVP(YOffset),
				CEREAL_NVP(YOffsetRandom),
				CEREAL_NVP(LineSplitAngle),
				CEREAL_NVP(MinLineLength),
				CEREAL_NVP(LineLinkLength),
				CEREAL_NVP(LineDirection),
				CEREAL_NVP(LoopDirectionType),

				CEREAL_NVP(DistortionEnable),
				CEREAL_NVP(DistortionMapToExport),
				CEREAL_NVP(DistortionMapAmount),
				CEREAL_NVP(Amount),
				CEREAL_NVP(Random),
				CEREAL_NVP(Cycles),
				CEREAL_NVP(CyclesRandom),
				CEREAL_NVP(Phase),
				CEREAL_NVP(PhaseRandom),

				CEREAL_NVP(SizeReductionEnable),
				CEREAL_NVP(SizeReductionCurveValues),
				CEREAL_NVP(AlphaReductionEnable),
				CEREAL_NVP(AlphaReductionCurveValues),

				CEREAL_NVP(ColorSpaceType),
				CEREAL_NVP(ColorRed),
				CEREAL_NVP(ColorGreen),
				CEREAL_NVP(ColorBlue),
				CEREAL_NVP(ColorHue),
				CEREAL_NVP(ColorSaturation),
				CEREAL_NVP(ColorValue)
			);
		}
	};

	struct ReductionSettingsNodeToExport : public Pcl4NativeDll::ReductionSettingsNode
	{
#ifdef PYBIND_INCLUDE
		static void registerClass(const pybind11::module_& m);

		std::optional<pybind11::object> ObjectReference; // -> ObjectPosX, Y, Z
#endif
		std::array<float, 256> CurveValues; // -> Curve

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(ReductionStart),
				CEREAL_NVP(ReductionEnd),
				CEREAL_NVP(CurveValues),
				CEREAL_NVP(ReferObject),
				CEREAL_NVP(ObjectPosX),
				CEREAL_NVP(ObjectPosY),
				CEREAL_NVP(ObjectPosZ)
			);
		}
	};

	struct TextureMapNodeToExport : public Pcl4NativeDll::TextureMapNode
	{
#ifdef PYBIND_INCLUDE
		static void registerClass(const pybind11::module_& m);

		std::array<float, 2> tillingXY; // -> tilingX, Y
		std::array<float, 2> offsetXY; // -> offsetX, Y
		std::optional<pybind11::object> ImageToExport;

		TextureMapSourceType source_type;

		TextureMapUVSource uv_source;
		TextureMapSelectionMode uv_selection_mode;
		int uv_index;
		std::string uv_name;

		TextureMapSelectionMode object_color_selection_mode;
		int object_color_index;
		std::string object_color_name;
#endif
		size_t ptr_offset;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(width),
				CEREAL_NVP(height),
				CEREAL_NVP(wrapModeU),
				CEREAL_NVP(wrapModeV),
				CEREAL_NVP(filterMode),
				CEREAL_NVP(tilingX),
				CEREAL_NVP(tilingY),
				CEREAL_NVP(offsetX),
				CEREAL_NVP(offsetY),
				CEREAL_NVP(uvSource),
				CEREAL_NVP(ptr_offset)
			);
			if (1 <= version) {
				archive(
					CEREAL_NVP(objectColorMode),
					CEREAL_NVP(exChannelIndex),
					CEREAL_NVP(exChannelID)
				);
			}
			else {
				objectColorMode = false;
				exChannelIndex = 0;
				exChannelID = 0;
			}
		}
	};

	struct LineFunctionsNodeToExport : public Pcl4NativeDll::LineFunctionsNode
	{
#ifdef PYBIND_INCLUDE
		static void registerClass(const pybind11::module_& m);

		std::vector<pybind11::object> TargetMaterialsToExport;
		std::vector<pybind11::object> DrawObjectsToExport;
		std::vector<pybind11::object> DrawMaterialsToExport;
		std::vector<pybind11::object> MaskObjectsToExport;
		std::vector<pybind11::object> MaskMaterialsToExport;

		std::array<float, 3> OutlineColorRGB;
		std::array<float, 3> ObjectColorRGB;
		std::array<float, 3> IntersectionColorRGB;
		std::array<float, 3> SmoothColorRGB;
		std::array<float, 3> MaterialColorRGB;
		std::array<float, 3> SelectedEdgeColorRGB;
		std::array<float, 3> NormalAngleColorRGB;
		std::array<float, 3> WireframeColorRGB;
#endif

		std::vector<int> TargetMaterialIdsToSerialize;
		std::vector<int> DrawObjectIdsToSerialize;
		std::vector<int> DrawMaterialIdsToSerialize;
		std::vector<int> MaskObjectIdsToSerialize;
		std::vector<int> MaskMaterialIdsToSerialize;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(TargetMaterialIdsToSerialize),
				CEREAL_NVP(OutlineOn),
				CEREAL_NVP(OutlineColorR),
				CEREAL_NVP(OutlineColorG),
				CEREAL_NVP(OutlineColorB),
				CEREAL_NVP(OutlineAmount),
				CEREAL_NVP(ObjectOn),
				CEREAL_NVP(ObjectColorR),
				CEREAL_NVP(ObjectColorG),
				CEREAL_NVP(ObjectColorB),
				CEREAL_NVP(ObjectAmount),
				CEREAL_NVP(IntersectionOn),
				CEREAL_NVP(IntersectionColorR),
				CEREAL_NVP(IntersectionColorG),
				CEREAL_NVP(IntersectionColorB),
				CEREAL_NVP(IntersectionAmount),
				CEREAL_NVP(SmoothOn),
				CEREAL_NVP(SmoothColorR),
				CEREAL_NVP(SmoothColorG),
				CEREAL_NVP(SmoothColorB),
				CEREAL_NVP(SmoothAmount),
				CEREAL_NVP(MaterialOn),
				CEREAL_NVP(MaterialColorR),
				CEREAL_NVP(MaterialColorG),
				CEREAL_NVP(MaterialColorB),
				CEREAL_NVP(MaterialAmount),
				CEREAL_NVP(SelectedEdgeOn),
				CEREAL_NVP(SelectedEdgeColorR),
				CEREAL_NVP(SelectedEdgeColorG),
				CEREAL_NVP(SelectedEdgeColorB),
				CEREAL_NVP(SelectedEdgeAmount),
				CEREAL_NVP(NormalAngleOn),
				CEREAL_NVP(NormalAngleColorR),
				CEREAL_NVP(NormalAngleColorG),
				CEREAL_NVP(NormalAngleColorB),
				CEREAL_NVP(NormalAngleAmount),
				CEREAL_NVP(WireframeOn),
				CEREAL_NVP(WireframeColorR),
				CEREAL_NVP(WireframeColorG),
				CEREAL_NVP(WireframeColorB),
				CEREAL_NVP(WireframeAmount),
				CEREAL_NVP(DisableIntersection),
				CEREAL_NVP(DrawHiddenLines),
				CEREAL_NVP(DrawHiddenLinesOfTarget),
				CEREAL_NVP(DrawObjectIdsToSerialize),
				CEREAL_NVP(DrawMaterialIdsToSerialize),
				CEREAL_NVP(MaskHiddenLinesOfTarget),
				CEREAL_NVP(MaskObjectIdsToSerialize),
				CEREAL_NVP(MaskMaterialIdsToSerialize)
			);
		}
	};

	struct LineRenderElementToExport : public Pcl4NativeDll::RenderElementsLineNode
	{
#ifdef PYBIND_INCLUDE
		static void registerClass(const pybind11::module_& m);

		enum class RenderElementType : int
		{
			Color = 0,
			Depth = 1,
		};

		pybind11::object Image;
		RenderElementType ElementType;
		std::array<float, 4> BackgroundColorRGBA;
		std::array<bool, 8> LinesetIDs;
#endif
		size_t ptr_offset;

		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(
				CEREAL_NVP(ptr_offset),
				CEREAL_NVP(isDrawVisibleLines),
				CEREAL_NVP(isDrawHiddenLines),
				CEREAL_NVP(isBackgroundColorEnabled),
				CEREAL_NVP(backgroundColorR),
				CEREAL_NVP(backgroundColorG),
				CEREAL_NVP(backgroundColorB),
				CEREAL_NVP(backgroundColorA),
				CEREAL_NVP(isDrawEdgeOutline),
				CEREAL_NVP(isDrawEdgeObject),
				CEREAL_NVP(isDrawEdgeISect),
				CEREAL_NVP(isDrawEdgeSmooth),
				CEREAL_NVP(isDrawEdgeMatID),
				CEREAL_NVP(isDrawEdgeSelectedEdge),
				CEREAL_NVP(isDrawEdgeNormal),
				CEREAL_NVP(isDrawEdgeWire),
				CEREAL_NVP(isDrawLineSetId1),
				CEREAL_NVP(isDrawLineSetId2),
				CEREAL_NVP(isDrawLineSetId3),
				CEREAL_NVP(isDrawLineSetId4),
				CEREAL_NVP(isDrawLineSetId5),
				CEREAL_NVP(isDrawLineSetId6),
				CEREAL_NVP(isDrawLineSetId7),
				CEREAL_NVP(isDrawLineSetId8),
				CEREAL_NVP(depthDrawModeEnabled),
				CEREAL_NVP(depthDrawMin),
				CEREAL_NVP(depthDrawMax),
				CEREAL_NVP(pixelBufferFormat)
			);
		}
	};


}

CEREAL_CLASS_VERSION(Nodes::LineNodeToExport, 1);
CEREAL_CLASS_VERSION(Nodes::TextureMapNodeToExport, 1);