#pragma once



namespace Pcl4NativeDll
{
	struct RenderInformation;
	struct LineNode;
	struct LineSetNode;
	struct LineFunctionsNode;
	struct BrushSettingsNode;
	struct ReductionSettingsNode;
	struct BrushDetailNode;
	struct TextureMapNode;
	struct LineGroupNode;
	struct RenderElementsLineNode;
	struct VectorOutputNode;

	enum class ColorSpace : int
	{
		Linear,
		sRGB,
	};

#pragma pack(4)
	struct RenderInformation
	{
		unsigned char* pixels;
		int width;
		int height;
		int pixelBufferFormat;
		float lineScale;
		ColorSpace inputColorSpace;
	};

	struct LineListNode
	{
		LineNode** LineList;
		int LineListLength;
		LineFunctionsNode** LineFunctionsList;
		int LineFunctionsListLength;
		int* DoubleSidedMaterialList;
		int DoubleSidedMaterialListLength;
		LineGroupNode** LineGroups;
		int LineGroupsLength;
	};

	struct LineNode
	{
		bool Active;
		int RenderPriority;
		LineSetNode** LineSets;
		int LineSetsLength;
		int LineSizeType;
		bool OutputToRenderElementsOnly;
		int LineOversampling;
		float LineAntialiasing;
		float LineOffscreenDistance;
		int LineRandomSeed;
		float ScaleEx;
		wchar_t* pName;	// for Vector Output
	};

	struct LineSetNode
	{
		bool On;
		int Id;

		int* Objects;
		int ObjectsLength;

		int* Materials;
		int MaterialsLength;

		bool WeldsEdges;
		bool MaskHiddenLines;

		BrushSettingsNode* VBrushSettings;

		bool VOutlineOn;
		bool VOutlineOpenEdge;
		bool VOutlineMergeGroups;
		bool VOutlineSpecificOn;
		BrushSettingsNode* VOutline;

		bool VObjectOn;
		bool VObjectOpenEdge;
		bool VObjectSpecificOn;
		BrushSettingsNode* VObject;

		bool VIntersectionOn;
		bool VIntersectionSelf;
		bool VIntersectionSpecificOn;
		BrushSettingsNode* VIntersection;

		bool VSmoothOn;
		bool VSmoothSpecificOn;
		BrushSettingsNode* VSmooth;

		bool VMaterialOn;
		bool VMaterialSpecificOn;
		BrushSettingsNode* VMaterial;

		bool VNormalAngleOn;
		bool VNormalAngleSpecificOn;
		BrushSettingsNode* VNormalAngle;

		float VNormalAngleMin;
		float VNormalAngleMax;

		bool VWireframeOn;
		bool VWireframeSpecificOn;
		BrushSettingsNode* VWireframe;

		bool VSelectedOn;
		bool VSelectedSpecificOn;
		BrushSettingsNode* VSelected;

		bool VSizeReductionOn;
		ReductionSettingsNode* VSizeReduction;

		bool VAlphaReductionOn;
		ReductionSettingsNode* VAlphaReduction;

		BrushSettingsNode* HBrushSettings;

		bool HOutlineOn;
		bool HOutlineOpenEdge;
		bool HOutlineMergeGroups;
		bool HOutlineSpecificOn;
		BrushSettingsNode* HOutline;

		bool HObjectOn;
		bool HObjectOpenEdge;
		bool HObjectSpecificOn;
		BrushSettingsNode* HObject;

		bool HIntersectionOn;
		bool HIntersectionSelf;
		bool HIntersectionSpecificOn;
		BrushSettingsNode* HIntersection;

		bool HSmoothOn;
		bool HSmoothSpecificOn;
		BrushSettingsNode* HSmooth;

		bool HMaterialOn;
		bool HMaterialSpecificOn;
		BrushSettingsNode* HMaterial;

		bool HNormalAngleOn;
		bool HNormalAngleSpecificOn;
		BrushSettingsNode* HNormalAngle;

		float HNormalAngleMin;
		float HNormalAngleMax;

		bool HWireframeOn;
		bool HWireframeSpecificOn;
		BrushSettingsNode* HWireframe;

		bool HSelectedOn;
		bool HSelectedSpecificOn;
		BrushSettingsNode* HSelected;

		bool HSizeReductionOn;
		ReductionSettingsNode* HSizeReduction;

		bool HAlphaReductionOn;
		ReductionSettingsNode* HAlphaReduction;

		wchar_t* pName;	// for Vector Output
		float UserDefinedColorR;	// for Vector Output
		float UserDefinedColorG;	// for Vector Output
		float UserDefinedColorB;	// for Vector Output
	};

	struct BrushSettingsNode
	{
		BrushDetailNode* BrushDetailNode;
		int BlendMode;
		float BlendAmount;
		float ColorR;
		float ColorG;
		float ColorB;
		float ColorA;
		TextureMapNode* ColorMap;
		float ColorMapOpacity;
		float Size;
		TextureMapNode* SizeMap;
		float SizeMapAmount;
	};

	// For BrushDetailNode
	enum class Brush : int
	{
		Normal = 0,
		Multiple,
		Simple
	};

	enum class Stroke : int
	{
		Normal = 0,
		Rake,
		Random
	};

	enum class Line : int
	{
		Full = 0,
		Dashed
	};

	enum class LoopDirection : int
	{
		Left = 0,
		Right
	};

	enum class ColorMode : int
	{
		RGB = 0,
		HSV
	};

	struct BrushDetailNode
	{
		Brush BrushType;
		TextureMapNode* BrushMap;
		float BrushMapOpacity;
		float Stretch;
		float StretchRandom;
		float Angle;
		float AngleRandom;
		float Groove;
		int GrooveNumber;
		float Size;
		float SizeRandom;
		float AntiAliasing;
		float HorizontalSpace;
		float HorizontalSpaceRandom;
		float VerticalSpace;
		float VerticalSpaceRandom;
		float ReductionStart;
		float ReductionEnd;
		Stroke StrokeType;
		Line LineType;
		float Length;
		float LengthRandom;
		float Space;
		float SpaceRandom;
		float StrokeSizeRandom;
		float Extend;
		float ExtendRandom;
		int LineCopy;
		int LineCopyRandom;
		float NormalOffset;
		float NormalOffsetRandom;
		float XOffset;
		float XOffsetRandom;
		float YOffset;
		float YOffsetRandom;
		float LineSplitAngle;
		float MinLineLength;
		float LineLinkLength;
		float LineDirection;
		LoopDirection LoopDirectionType;
		bool DistortionEnable;
		TextureMapNode* DistortionMap;
		float DistortionMapAmount;
		float Amount;
		float Random;
		float Cycles;
		float CyclesRandom;
		float Phase;
		float PhaseRandom;

		bool SizeReductionEnable;
		float* SizeReductionCurve;
		bool AlphaReductionEnable;
		float* AlphaReductionCurve;

		int ColorModeType;
		float ColorRed;
		float ColorGreen;
		float ColorBlue;
		float ColorHue;
		float ColorSaturation;
		float ColorValue;
	};


	struct ReductionSettingsNode
	{
		float ReductionStart;
		float ReductionEnd;
		float* Curve;
		bool ReferObject;
		float ObjectPosX;
		float ObjectPosY;
		float ObjectPosZ;
	};

	struct LineFunctionsNode
	{
		int* TargetMaterialIds;
		int TargetMaterialsLength;

		bool OutlineOn;
		float OutlineColorR;
		float OutlineColorG;
		float OutlineColorB;
		float OutlineAmount;

		bool ObjectOn;
		float ObjectColorR;
		float ObjectColorG;
		float ObjectColorB;
		float ObjectAmount;

		bool IntersectionOn;
		float IntersectionColorR;
		float IntersectionColorG;
		float IntersectionColorB;
		float IntersectionAmount;

		bool SmoothOn;
		float SmoothColorR;
		float SmoothColorG;
		float SmoothColorB;
		float SmoothAmount;

		bool MaterialOn;
		float MaterialColorR;
		float MaterialColorG;
		float MaterialColorB;
		float MaterialAmount;

		bool SelectedEdgeOn;
		float SelectedEdgeColorR;
		float SelectedEdgeColorG;
		float SelectedEdgeColorB;
		float SelectedEdgeAmount;

		bool NormalAngleOn;
		float NormalAngleColorR;
		float NormalAngleColorG;
		float NormalAngleColorB;
		float NormalAngleAmount;

		bool WireframeOn;
		float WireframeColorR;
		float WireframeColorG;
		float WireframeColorB;
		float WireframeAmount;

		bool DisableIntersection;
		bool DrawHiddenLines;
		bool DrawHiddenLinesOfTarget;

		int* DrawObjectIds;
		int DrawObjectsLength;
		int* DrawMaterialIds;
		int DrawMaterialLength;

		bool MaskHiddenLinesOfTarget;

		int* MaskObjectIds;
		int MaskObjectsLength;
		int* MaskMaterialIds;
		int MaskMaterialsLength;

	};

	// For TextureMapNode
	enum class TextureWrapMode : int
	{
		Repeat,
		Clamp,
		Mirror,
		MirrorOnce
	};

	enum class TextureFilterMode : int
	{
		Point,
		Bilinear,
		Trilinear
	};

	enum class TextureUVSource : int
	{
		Screen = 0,
		MeshObject1 = 1,
		MeshObject2,
		MeshObject3,
		MeshObject4,
		MeshObject5,
		MeshObject6,
		MeshObject7,
		MeshObject8
	};

	struct TextureMapNode
	{
		unsigned int* texturePixels;
		int width;
		int height;
		TextureWrapMode wrapModeU;
		TextureWrapMode wrapModeV;
		TextureFilterMode filterMode;
		float tilingX;
		float tilingY;
		float offsetX;
		float offsetY;
		TextureUVSource uvSource;
		bool objectColorMode;
		int exChannelIndex;
		int exChannelID;
	};

	struct LineGroupNode
	{
		int* objectIds;
		int objectIdsLength;
	};

	struct RenderElementsInfo
	{
		RenderElementsLineNode** lines;
		int linesLength;
	};

	struct RenderElementsLineNode
	{
		unsigned int* targetTexturePixels;
		bool isDrawVisibleLines;
		bool isDrawHiddenLines;
		bool isBackgroundColorEnabled;
		float backgroundColorR;
		float backgroundColorG;
		float backgroundColorB;
		float backgroundColorA;
		bool isDrawEdgeOutline;
		bool isDrawEdgeObject;
		bool isDrawEdgeISect;
		bool isDrawEdgeSmooth;
		bool isDrawEdgeMatID;
		bool isDrawEdgeSelectedEdge;
		bool isDrawEdgeNormal;
		bool isDrawEdgeWire;
		bool isDrawLineSetId1;
		bool isDrawLineSetId2;
		bool isDrawLineSetId3;
		bool isDrawLineSetId4;
		bool isDrawLineSetId5;
		bool isDrawLineSetId6;
		bool isDrawLineSetId7;
		bool isDrawLineSetId8;
		bool depthDrawModeEnabled;
		float depthDrawMin;
		float depthDrawMax;
		int pixelBufferFormat;
	};


	struct VectorOutputsInfo
	{
		VectorOutputNode** outputs;
		int outputsLength;
	};

	enum class VectorOutputType : int
	{
		AIEPS,
		EPS,
		PLD,
	};

	struct VectorOutputNode
	{
		wchar_t* outputPath;
		VectorOutputType outputType;
		bool isDrawVisibleLines;
		bool isDrawHiddenLines;
		bool isDrawEdgeOutline;
		bool isDrawEdgeObject;
		bool isDrawEdgeISect;
		bool isDrawEdgeSmooth;
		bool isDrawEdgeMatID;
		bool isDrawEdgeSelectedEdge;
		bool isDrawEdgeNormal;
		bool isDrawEdgeWire;
		bool isDrawLineSetId1;
		bool isDrawLineSetId2;
		bool isDrawLineSetId3;
		bool isDrawLineSetId4;
		bool isDrawLineSetId5;
		bool isDrawLineSetId6;
		bool isDrawLineSetId7;
		bool isDrawLineSetId8;
	};

#pragma pack()
}