#include "Nodes.h"

namespace py = pybind11;

void Nodes::registerEnums(const pybind11::module_& m)
{
	// ����: ������enum�^��V�����ǉ�����Ƃ��́Apython���Ɍ��J���閼�O�Ƀv���t�B�b�N�X"pcl4_enum_"��t���Ă��������B
	//       Python���Ō^��enum�ł��邩�ǂ����𔻕ʂ��邽�߂Ƀv���t�B�b�N�X��g���Ă��܂��B

	using namespace Pcl4NativeDll;

	py::enum_<LineSizeType>(m, "pcl4_enum_line_size_type")
		.value("absolute", LineSizeType::Absolute)
		.value("relative", LineSizeType::Relative)
		;

	py::enum_<Brush>(m, "pcl4_enum_brush_type")
		.value("normal", Brush::Normal)
		.value("multiple", Brush::Multiple)
		.value("simple", Brush::Simple)
		;

	py::enum_<Stroke>(m, "pcl4_enum_stroke_type")
		.value("normal", Stroke::Normal)
		.value("rake", Stroke::Rake)
		.value("random", Stroke::Random)
		;

	py::enum_<Line>(m, "pcl4_enum_line_type")
		.value("full", Line::Full)
		.value("dashed", Line::Dashed)
		;

	py::enum_<LoopDirectionType>(m, "pcl4_enum_loop_direction")
		.value("clockwise", LoopDirectionType::Clockwise)
		.value("anticlockwise", LoopDirectionType::Anticlockwise)
		;

	py::enum_<ColorMode>(m, "pcl4_enum_color_mode_type")
		.value("RGB", ColorMode::RGB)
		.value("HSV", ColorMode::HSV)
		;

	py::enum_<TextureMapSourceType>(m, "pcl4_enum_texture_map_source_type")
		.value("image", TextureMapSourceType::Image)
		.value("object_color", TextureMapSourceType::ObjectColor)
		;

	py::enum_<TextureMapUVSource>(m, "pcl4_enum_texture_map_uv_source")
		.value("screen", TextureMapUVSource::Screen)
		.value("object_uv", TextureMapUVSource::ObjectUV)
		;
	
	py::enum_<TextureMapSelectionMode>(m, "pcl4_enum_texture_map_selection_mode")
		.value("index", TextureMapSelectionMode::Index)
		.value("name", TextureMapSelectionMode::Name)
		;

	py::enum_<TextureWrapMode>(m, "pcl4_enum_texture_wrap_mode")
		.value("repeat", TextureWrapMode::Repeat)
		.value("clamp", TextureWrapMode::Clamp)
		.value("mirror", TextureWrapMode::Mirror)
		.value("mirror_once", TextureWrapMode::MirrorOnce)
		.value("clip", TextureWrapMode::Clip)
		;

	py::enum_<TextureFilterMode>(m, "pcl4_enum_texture_filter_mode")
		.value("point", TextureFilterMode::Point)
		.value("bilinear", TextureFilterMode::Bilinear)
		;

	py::enum_<LineRenderElementToExport::RenderElementType>(m, "pcl4_enum_render_element_type")
		.value("COLOR", LineRenderElementToExport::RenderElementType::Color)
		.value("DEPTH", LineRenderElementToExport::RenderElementType::Depth)
		;

	py::enum_<VectorOutputType> (m, "pcl4_enum_vector_putput_type")
		.value("AIEPS", VectorOutputType::AIEPS)
		.value("EPS", VectorOutputType::EPS)
		.value("PLD", VectorOutputType::PLD)
		;
}

void Nodes::LineNodeToExport::registerClass(const pybind11::module_& m)
{
	py::class_<LineNodeToExport, std::shared_ptr<LineNodeToExport>>(m, "line_node")
		.def(py::init<>())
		.def_readwrite("is_active", &LineNodeToExport::Active)
		.def_readwrite("render_priority", &LineNodeToExport::RenderPriority)
		.def_readwrite("line_sets", &LineNodeToExport::LineSetNodesToExport)
		.def_readwrite("line_size_type", &LineNodeToExport::LineSizeTypeValue)
		.def_readwrite("is_output_to_render_elements_only", &LineNodeToExport::OutputToRenderElementsOnly)
		.def_readwrite("over_sampling", &LineNodeToExport::LineOversampling)
		.def_readwrite("antialiasing", &LineNodeToExport::LineAntialiasing)
		.def_readwrite("off_screen_distance", &LineNodeToExport::LineOffscreenDistance)
		.def_readwrite("random_seed", &LineNodeToExport::LineRandomSeed)
		.def_readwrite("_scale_ex", &LineNodeToExport::ScaleEx)
		.def_readwrite("node_name", &LineNodeToExport::Name)
		;
}

void Nodes::LineSetNodeToExport::registerClass(const pybind11::module_& m)
{
	py::class_<LineSetNodeToExport, std::shared_ptr<LineSetNodeToExport>>(m, "line_set_node")
		.def(py::init<>())
		.def_readwrite("is_on", &LineSetNodeToExport::On)
		.def_readwrite("lineset_id", &LineSetNodeToExport::Id)
		.def_readwrite("objects", &LineSetNodeToExport::ObjectsToExport)
		.def_readwrite("materials", &LineSetNodeToExport::MaterialsToExport)
		.def_readwrite("is_weld_edges", &LineSetNodeToExport::WeldsEdges)
		.def_readwrite("is_mask_hidden_lines", &LineSetNodeToExport::MaskHiddenLines)

		.def_readwrite("v_brush_settings", &LineSetNodeToExport::VBrushSettingsToExport)

		.def_readwrite("v_outline_on", &LineSetNodeToExport::VOutlineOn)
		.def_readwrite("v_outline_open", &LineSetNodeToExport::VOutlineOpenEdge)
		.def_readwrite("v_outline_merge_groups", &LineSetNodeToExport::VOutlineMergeGroups)
		.def_readwrite("v_outline_specific_on", &LineSetNodeToExport::VOutlineSpecificOn)
		.def_readwrite("v_outline_brush_settings", &LineSetNodeToExport::VOutlineToExport)

		.def_readwrite("v_object_on", &LineSetNodeToExport::VObjectOn)
		.def_readwrite("v_object_open", &LineSetNodeToExport::VObjectOpenEdge)
		.def_readwrite("v_object_specific_on", &LineSetNodeToExport::VObjectSpecificOn)
		.def_readwrite("v_object_brush_settings", &LineSetNodeToExport::VObjectToExport)

		.def_readwrite("v_intersection_on", &LineSetNodeToExport::VIntersectionOn)
		.def_readwrite("v_intersection_self", &LineSetNodeToExport::VIntersectionSelf)
		.def_readwrite("v_intersection_specific_on", &LineSetNodeToExport::VIntersectionSpecificOn)
		.def_readwrite("v_intersection_brush_settings", &LineSetNodeToExport::VIntersectionToExport)

		.def_readwrite("v_smooth_on", &LineSetNodeToExport::VSmoothOn)
		.def_readwrite("v_smooth_specific_on", &LineSetNodeToExport::VSmoothSpecificOn)
		.def_readwrite("v_smooth_brush_settings", &LineSetNodeToExport::VSmoothToExport)

		.def_readwrite("v_material_on", &LineSetNodeToExport::VMaterialOn)
		.def_readwrite("v_material_specific_on", &LineSetNodeToExport::VMaterialSpecificOn)
		.def_readwrite("v_material_brush_settings", &LineSetNodeToExport::VMaterialToExport)

		.def_readwrite("v_normal_angle_on", &LineSetNodeToExport::VNormalAngleOn)
		.def_readwrite("v_normal_angle_specific_on", &LineSetNodeToExport::VNormalAngleSpecificOn)
		.def_readwrite("v_normal_angle_brush_settings", &LineSetNodeToExport::VNormalAngleToExport)
		.def_readwrite("v_normal_angle_min", &LineSetNodeToExport::VNormalAngleMin)
		.def_readwrite("v_normal_angle_max", &LineSetNodeToExport::VNormalAngleMax)

		.def_readwrite("v_wireframe_on", &LineSetNodeToExport::VWireframeOn)
		.def_readwrite("v_wireframe_specific_on", &LineSetNodeToExport::VWireframeSpecificOn)
		.def_readwrite("v_wireframe_brush_settings", &LineSetNodeToExport::VWireframeToExport)

		.def_readwrite("v_selected_on", &LineSetNodeToExport::VSelectedOn)
		.def_readwrite("v_selected_specific_on", &LineSetNodeToExport::VSelectedSpecificOn)
		.def_readwrite("v_selected_brush_settings", &LineSetNodeToExport::VSelectedToExport)

		.def_readwrite("v_size_reduction_on", &LineSetNodeToExport::VSizeReductionOn)
		.def_readwrite("v_size_reduction_settings", &LineSetNodeToExport::VSizeReductionToExport)
		.def_readwrite("v_alpha_reduction_on", &LineSetNodeToExport::VAlphaReductionOn)
		.def_readwrite("v_alpha_reduction_settings", &LineSetNodeToExport::VAlphaReductionToExport)


		.def_readwrite("h_brush_settings", &LineSetNodeToExport::HBrushSettingsToExport)

		.def_readwrite("h_outline_on", &LineSetNodeToExport::HOutlineOn)
		.def_readwrite("h_outline_open", &LineSetNodeToExport::HOutlineOpenEdge)
		.def_readwrite("h_outline_merge_groups", &LineSetNodeToExport::HOutlineMergeGroups)
		.def_readwrite("h_outline_specific_on", &LineSetNodeToExport::HOutlineSpecificOn)
		.def_readwrite("h_outline_brush_settings", &LineSetNodeToExport::HOutlineToExport)

		.def_readwrite("h_object_on", &LineSetNodeToExport::HObjectOn)
		.def_readwrite("h_object_open", &LineSetNodeToExport::HObjectOpenEdge)
		.def_readwrite("h_object_specific_on", &LineSetNodeToExport::HObjectSpecificOn)
		.def_readwrite("h_object_brush_settings", &LineSetNodeToExport::HObjectToExport)

		.def_readwrite("h_intersection_on", &LineSetNodeToExport::HIntersectionOn)
		.def_readwrite("h_intersection_self", &LineSetNodeToExport::HIntersectionSelf)
		.def_readwrite("h_intersection_specific_on", &LineSetNodeToExport::HIntersectionSpecificOn)
		.def_readwrite("h_intersection_brush_settings", &LineSetNodeToExport::HIntersectionToExport)

		.def_readwrite("h_smooth_on", &LineSetNodeToExport::HSmoothOn)
		.def_readwrite("h_smooth_specific_on", &LineSetNodeToExport::HSmoothSpecificOn)
		.def_readwrite("h_smooth_brush_settings", &LineSetNodeToExport::HSmoothToExport)

		.def_readwrite("h_material_on", &LineSetNodeToExport::HMaterialOn)
		.def_readwrite("h_material_specific_on", &LineSetNodeToExport::HMaterialSpecificOn)
		.def_readwrite("h_material_brush_settings", &LineSetNodeToExport::HMaterialToExport)

		.def_readwrite("h_normal_angle_on", &LineSetNodeToExport::HNormalAngleOn)
		.def_readwrite("h_normal_angle_specific_on", &LineSetNodeToExport::HNormalAngleSpecificOn)
		.def_readwrite("h_normal_angle_brush_settings", &LineSetNodeToExport::HNormalAngleToExport)
		.def_readwrite("h_normal_angle_min", &LineSetNodeToExport::HNormalAngleMin)
		.def_readwrite("h_normal_angle_max", &LineSetNodeToExport::HNormalAngleMax)

		.def_readwrite("h_wireframe_on", &LineSetNodeToExport::HWireframeOn)
		.def_readwrite("h_wireframe_specific_on", &LineSetNodeToExport::HWireframeSpecificOn)
		.def_readwrite("h_wireframe_brush_settings", &LineSetNodeToExport::HWireframeToExport)

		.def_readwrite("h_selected_on", &LineSetNodeToExport::HSelectedOn)
		.def_readwrite("h_selected_specific_on", &LineSetNodeToExport::HSelectedSpecificOn)
		.def_readwrite("h_selected_brush_settings", &LineSetNodeToExport::HSelectedToExport)

		.def_readwrite("h_size_reduction_on", &LineSetNodeToExport::HSizeReductionOn)
		.def_readwrite("h_size_reduction_settings", &LineSetNodeToExport::HSizeReductionToExport)
		.def_readwrite("h_alpha_reduction_on", &LineSetNodeToExport::HAlphaReductionOn)
		.def_readwrite("h_alpha_reduction_settings", &LineSetNodeToExport::HAlphaReductionToExport)

		.def_readwrite("node_name", &LineSetNodeToExport::Name)
		.def_readwrite("user_defined_color", &LineSetNodeToExport::UserDefinedColorRGB)
		;
}

void Nodes::BrushSettingsNodeToExport::registerClass(const pybind11::module_& m)
{
	py::class_<BrushSettingsNodeToExport, std::shared_ptr<BrushSettingsNodeToExport>>(m, "brush_settings_node")
		.def(py::init<>())
		.def_readwrite("brush_detail_node", &BrushSettingsNodeToExport::BrushDetailNodeToExport)
		.def_readwrite("blend_amount", &BrushSettingsNodeToExport::BlendAmount)
		.def_readwrite("brush_color", &BrushSettingsNodeToExport::ColorRGB)
		.def_readwrite("color_map_on", &BrushSettingsNodeToExport::ColorMapOn)
		.def_readwrite("color_map", &BrushSettingsNodeToExport::ColorMapToExport)
		.def_readwrite("color_map_opacity", &BrushSettingsNodeToExport::ColorMapOpacity)
		.def_readwrite("size", &BrushSettingsNodeToExport::Size)
		.def_readwrite("size_map_on", &BrushSettingsNodeToExport::SizeMapOn)
		.def_readwrite("size_map", &BrushSettingsNodeToExport::SizeMapToExport)
		.def_readwrite("size_map_amount", &BrushSettingsNodeToExport::SizeMapAmount)
		;
}

void Nodes::BrushDetailNodeToExport::registerClass(const pybind11::module_& m)
{
	py::class_<BrushDetailNodeToExport, std::shared_ptr<BrushDetailNodeToExport>>(m, "brush_detail_node")
		.def(py::init<>())
		.def_readwrite("brush_type", &BrushDetailNodeToExport::BrushType)
		.def_readwrite("brush_map_on", &BrushDetailNodeToExport::BrushMapOn)
		.def_readwrite("brush_map", &BrushDetailNodeToExport::BrushMapToExport)
		.def_readwrite("brush_map_opacity", &BrushDetailNodeToExport::BrushMapOpacity)
		.def_readwrite("stretch", &BrushDetailNodeToExport::Stretch)
		.def_readwrite("stretch_random", &BrushDetailNodeToExport::StretchRandom)
		.def_readwrite("angle", &BrushDetailNodeToExport::Angle)
		.def_readwrite("angle_random", &BrushDetailNodeToExport::AngleRandom)
		.def_readwrite("groove", &BrushDetailNodeToExport::Groove)
		.def_readwrite("groove_number", &BrushDetailNodeToExport::GrooveNumber)
		.def_readwrite("size", &BrushDetailNodeToExport::Size)
		.def_readwrite("size_random", &BrushDetailNodeToExport::SizeRandom)
		.def_readwrite("antialiasing", &BrushDetailNodeToExport::AntiAliasing)
		.def_readwrite("horizontal_space", &BrushDetailNodeToExport::HorizontalSpace)
		.def_readwrite("horizontal_space_random", &BrushDetailNodeToExport::HorizontalSpaceRandom)
		.def_readwrite("vertical_space", &BrushDetailNodeToExport::VerticalSpace)
		.def_readwrite("vertical_space_random", &BrushDetailNodeToExport::VerticalSpaceRandom)
		.def_readwrite("reduction_start", &BrushDetailNodeToExport::ReductionStart)
		.def_readwrite("reduction_end", &BrushDetailNodeToExport::ReductionEnd)

		.def_readwrite("stroke_type", &BrushDetailNodeToExport::StrokeType)
		.def_readwrite("line_type", &BrushDetailNodeToExport::LineType)
		.def_readwrite("length", &BrushDetailNodeToExport::Length)
		.def_readwrite("length_random", &BrushDetailNodeToExport::LengthRandom)
		.def_readwrite("space", &BrushDetailNodeToExport::Space)
		.def_readwrite("space_random", &BrushDetailNodeToExport::SpaceRandom)
		.def_readwrite("stroke_size_random", &BrushDetailNodeToExport::StrokeSizeRandom)
		.def_readwrite("extend", &BrushDetailNodeToExport::Extend)
		.def_readwrite("extend_random", &BrushDetailNodeToExport::ExtendRandom)
		.def_readwrite("line_copy", &BrushDetailNodeToExport::LineCopy)
		.def_readwrite("line_copy_random", &BrushDetailNodeToExport::LineCopyRandom)
		.def_readwrite("normal_offset", &BrushDetailNodeToExport::NormalOffset)
		.def_readwrite("normal_offset_random", &BrushDetailNodeToExport::NormalOffsetRandom)
		.def_readwrite("x_offset", &BrushDetailNodeToExport::XOffset)
		.def_readwrite("x_offset_random", &BrushDetailNodeToExport::XOffsetRandom)
		.def_readwrite("y_offset", &BrushDetailNodeToExport::YOffset)
		.def_readwrite("y_offset_random", &BrushDetailNodeToExport::YOffsetRandom)
		.def_readwrite("line_split_angle", &BrushDetailNodeToExport::LineSplitAngle)
		.def_readwrite("min_line_length", &BrushDetailNodeToExport::MinLineLength)
		.def_readwrite("line_link_length", &BrushDetailNodeToExport::LineLinkLength)
		.def_readwrite("line_direction", &BrushDetailNodeToExport::LineDirection)
		.def_readwrite("loop_direction_type", &BrushDetailNodeToExport::LoopDirectionTypeValue)

		.def_readwrite("distortion_enabled", &BrushDetailNodeToExport::DistortionEnable)
		.def_readwrite("distortion_map_on", &BrushDetailNodeToExport::DistortionMapOn)
		.def_readwrite("distortion_map", &BrushDetailNodeToExport::DistortionMapToExport)
		.def_readwrite("distortion_map_amount", &BrushDetailNodeToExport::DistortionMapAmount)
		.def_readwrite("distortion_amount", &BrushDetailNodeToExport::Amount)
		.def_readwrite("distortion_random", &BrushDetailNodeToExport::Random)
		.def_readwrite("distortion_cycles", &BrushDetailNodeToExport::Cycles)
		.def_readwrite("distortion_cycles_random", &BrushDetailNodeToExport::CyclesRandom)
		.def_readwrite("distortion_phase", &BrushDetailNodeToExport::Phase)
		.def_readwrite("distortion_phase_random", &BrushDetailNodeToExport::PhaseRandom)

		.def_readwrite("size_reduction_enabled", &BrushDetailNodeToExport::SizeReductionEnable)
		.def_readwrite("size_reduction_curve", &BrushDetailNodeToExport::SizeReductionCurveValues)
		.def_readwrite("alpha_reduction_enabled", &BrushDetailNodeToExport::AlphaReductionEnable)
		.def_readwrite("alpha_reduction_curve", &BrushDetailNodeToExport::AlphaReductionCurveValues)

		.def_readwrite("color_space_type", &BrushDetailNodeToExport::ColorModeTypeValue)
		.def_readwrite("color_space_red", &BrushDetailNodeToExport::ColorRed)
		.def_readwrite("color_space_green", &BrushDetailNodeToExport::ColorGreen)
		.def_readwrite("color_space_blue", &BrushDetailNodeToExport::ColorBlue)
		.def_readwrite("color_space_hue", &BrushDetailNodeToExport::ColorHue)
		.def_readwrite("color_space_saturation", &BrushDetailNodeToExport::ColorSaturation)
		.def_readwrite("color_space_value", &BrushDetailNodeToExport::ColorValue)
		;
}


void Nodes::ReductionSettingsNodeToExport::registerClass(const pybind11::module_& m)
{
	py::class_<ReductionSettingsNodeToExport, std::shared_ptr<ReductionSettingsNodeToExport>>(m, "reduction_settings_node")
		.def(py::init<>())
		.def_readwrite("reduction_start", &ReductionSettingsNodeToExport::ReductionStart)
		.def_readwrite("reduction_end", &ReductionSettingsNodeToExport::ReductionEnd)
		.def_readwrite("curve", &ReductionSettingsNodeToExport::CurveValues)
		.def_readwrite("refer_object_on", &ReductionSettingsNodeToExport::ReferObject)
		.def_readwrite("object_reference", &ReductionSettingsNodeToExport::ObjectReference)
		;
}

void Nodes::TextureMapNodeToExport::registerClass(const pybind11::module_& m)
{
	py::class_<TextureMapNodeToExport, std::shared_ptr<TextureMapNodeToExport>>(m, "texture_map_node")
		.def(py::init<>())
		.def_readwrite("image", &TextureMapNodeToExport::ImageToExport)
		.def_readwrite("wrap_mode_u", &TextureMapNodeToExport::wrapModeU)
		.def_readwrite("wrap_mode_v", &TextureMapNodeToExport::wrapModeV)
		.def_readwrite("filter_mode", &TextureMapNodeToExport::filterMode)
		.def_readwrite("tiling", &TextureMapNodeToExport::tillingXY)
		.def_readwrite("offset", &TextureMapNodeToExport::offsetXY)
		.def_readwrite("source_type", &TextureMapNodeToExport::source_type)
		.def_readwrite("uv_source", &TextureMapNodeToExport::uv_source)
		.def_readwrite("uv_selection_mode", &TextureMapNodeToExport::uv_selection_mode)
		.def_readwrite("uv_index", &TextureMapNodeToExport::uv_index)
		.def_readwrite("uv_name", &TextureMapNodeToExport::uv_name)
		.def_readwrite("object_color_selection_mode", &TextureMapNodeToExport::object_color_selection_mode)
		.def_readwrite("object_color_index", &TextureMapNodeToExport::object_color_index)
		.def_readwrite("object_color_name", &TextureMapNodeToExport::object_color_name)
		.def_readwrite("rotation", &TextureMapNodeToExport::rotation)
		;
}


void Nodes::LineFunctionsNodeToExport::registerClass(const pybind11::module_& m)
{
	py::class_<LineFunctionsNodeToExport, std::shared_ptr<LineFunctionsNodeToExport>>(m, "line_functions_node")
		.def(py::init<>())
		.def_readwrite("outline_on", &LineFunctionsNodeToExport::OutlineOn)
		.def_readwrite("outline_color", &LineFunctionsNodeToExport::OutlineColorRGB)
		.def_readwrite("outline_amount", &LineFunctionsNodeToExport::OutlineAmount)
		.def_readwrite("object_on", &LineFunctionsNodeToExport::ObjectOn)
		.def_readwrite("object_color", &LineFunctionsNodeToExport::ObjectColorRGB)
		.def_readwrite("object_amount", &LineFunctionsNodeToExport::ObjectAmount)
		.def_readwrite("intersection_on", &LineFunctionsNodeToExport::IntersectionOn)
		.def_readwrite("intersection_color", &LineFunctionsNodeToExport::IntersectionColorRGB)
		.def_readwrite("intersection_amount", &LineFunctionsNodeToExport::IntersectionAmount)
		.def_readwrite("smooth_on", &LineFunctionsNodeToExport::SmoothOn)
		.def_readwrite("smooth_color", &LineFunctionsNodeToExport::SmoothColorRGB)
		.def_readwrite("smooth_amount", &LineFunctionsNodeToExport::SmoothAmount)
		.def_readwrite("material_on", &LineFunctionsNodeToExport::MaterialOn)
		.def_readwrite("material_color", &LineFunctionsNodeToExport::MaterialColorRGB)
		.def_readwrite("material_amount", &LineFunctionsNodeToExport::MaterialAmount)
		.def_readwrite("selected_edge_on", &LineFunctionsNodeToExport::SelectedEdgeOn)
		.def_readwrite("selected_edge_color", &LineFunctionsNodeToExport::SelectedEdgeColorRGB)
		.def_readwrite("selected_edge_amount", &LineFunctionsNodeToExport::SelectedEdgeAmount)
		.def_readwrite("normal_angle_on", &LineFunctionsNodeToExport::NormalAngleOn)
		.def_readwrite("normal_angle_color", &LineFunctionsNodeToExport::NormalAngleColorRGB)
		.def_readwrite("normal_angle_amount", &LineFunctionsNodeToExport::NormalAngleAmount)
		.def_readwrite("wireframe_on", &LineFunctionsNodeToExport::WireframeOn)
		.def_readwrite("wireframe_color", &LineFunctionsNodeToExport::WireframeColorRGB)
		.def_readwrite("wireframe_amount", &LineFunctionsNodeToExport::WireframeAmount)
		.def_readwrite("disable_intersection", &LineFunctionsNodeToExport::DisableIntersection)
		.def_readwrite("draw_hidden_lines", &LineFunctionsNodeToExport::DrawHiddenLines)
		.def_readwrite("draw_hidden_lines_of_targets", &LineFunctionsNodeToExport::DrawHiddenLinesOfTarget)
		.def_readwrite("draw_hidden_lines_of_targets_objects", &LineFunctionsNodeToExport::DrawObjectsToExport)
		.def_readwrite("draw_hidden_lines_of_targets_materials", &LineFunctionsNodeToExport::DrawMaterialsToExport)
		.def_readwrite("mask_hidden_lines_of_targets", &LineFunctionsNodeToExport::MaskHiddenLinesOfTarget)
		.def_readwrite("mask_hidden_lines_of_targets_objects", &LineFunctionsNodeToExport::MaskObjectsToExport)
		.def_readwrite("mask_hidden_lines_of_targets_materials", &LineFunctionsNodeToExport::MaskMaterialsToExport)
		.def_readwrite("_target_materials", &LineFunctionsNodeToExport::TargetMaterialsToExport)
		;
}


void Nodes::LineRenderElementToExport::registerClass(const pybind11::module_& m)
{
	py::class_<LineRenderElementToExport, std::shared_ptr<LineRenderElementToExport>>(m, "line_render_element")
		.def(py::init<>())
		.def_readwrite("_image", &LineRenderElementToExport::Image)
		.def_readwrite("element_type", &LineRenderElementToExport::ElementType)
		.def_readwrite("z_min", &LineRenderElementToExport::depthDrawMin)
		.def_readwrite("z_max", &LineRenderElementToExport::depthDrawMax)
		.def_readwrite("enalbe_background_color", &LineRenderElementToExport::isBackgroundColorEnabled)
		.def_readwrite("background_color", &LineRenderElementToExport::BackgroundColorRGBA)
		.def_readwrite("visible_lines_on", &LineRenderElementToExport::isDrawVisibleLines)
		.def_readwrite("hidden_lines_on", &LineRenderElementToExport::isDrawHiddenLines)
		.def_readwrite("outline_on", &LineRenderElementToExport::isDrawEdgeOutline)
		.def_readwrite("object_on", &LineRenderElementToExport::isDrawEdgeObject)
		.def_readwrite("intersection_on", &LineRenderElementToExport::isDrawEdgeISect)
		.def_readwrite("smoothing_on", &LineRenderElementToExport::isDrawEdgeSmooth)
		.def_readwrite("material_id_on", &LineRenderElementToExport::isDrawEdgeMatID)
		.def_readwrite("selected_edges_on", &LineRenderElementToExport::isDrawEdgeSelectedEdge)
		.def_readwrite("normal_angle_on", &LineRenderElementToExport::isDrawEdgeNormal)
		.def_readwrite("wireframe_on", &LineRenderElementToExport::isDrawEdgeWire)
		.def_readwrite("line_set_ids", &LineRenderElementToExport::LinesetIDs)
		;
}


void Nodes::VectorOutputToExport::registerClass(const pybind11::module_& m)
{
	py::class_<VectorOutputToExport, std::shared_ptr<VectorOutputToExport>>(m, "vector_output")
		.def(py::init<>())
		.def_readwrite("output_path", &VectorOutputToExport::path)
		.def_readwrite("file_type", &VectorOutputToExport::outputType)
		.def_readwrite("visible_lines_on", &VectorOutputToExport::isDrawVisibleLines)
		.def_readwrite("hidden_lines_on", &VectorOutputToExport::isDrawHiddenLines)
		.def_readwrite("outline_on", &VectorOutputToExport::isDrawEdgeOutline)
		.def_readwrite("object_on", &VectorOutputToExport::isDrawEdgeObject)
		.def_readwrite("intersection_on", &VectorOutputToExport::isDrawEdgeISect)
		.def_readwrite("smoothing_on", &VectorOutputToExport::isDrawEdgeSmooth)
		.def_readwrite("material_id_on", &VectorOutputToExport::isDrawEdgeMatID)
		.def_readwrite("selected_edges_on", &VectorOutputToExport::isDrawEdgeSelectedEdge)
		.def_readwrite("normal_angle_on", &VectorOutputToExport::isDrawEdgeNormal)
		.def_readwrite("wireframe_on", &VectorOutputToExport::isDrawEdgeWire)
		.def_readwrite("line_set_ids", &VectorOutputToExport::LinesetIDs)
		;
}