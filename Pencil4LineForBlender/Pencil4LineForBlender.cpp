#include "pybind11/pybind11.h"
#include "pybind11/operators.h"
#include "pybind11/stl.h"
#include "pybind11/numpy.h"
#include "intermCamera.h"
#include "intermRenderInstance.h"
#include "intermContext.h"
#include "intermMeshDataAccessor.h"
#include "intermDrawOptions.h"
#include "intermBlenderVersion.h"
#include "intermDataHash.h"
#include "blrnaImage.h"
#include "blrnaCamera.h"	
#include "Nodes.h"
#include "commitHash.h"

namespace py = pybind11;

#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>


#ifdef _WINDOWS
#if defined PYTHON39
PYBIND11_MODULE(pencil4line_for_blender_win64_39, m)
#elif defined PYTHON310
PYBIND11_MODULE(pencil4line_for_blender_win64_310, m)
#elif defined PYTHON311
#ifndef NEW_PROPERTY_RNA
PYBIND11_MODULE(pencil4line_for_blender_win64_311, m)
#else
PYBIND11_MODULE(pencil4line_for_blender_win64_311_450, m)
#endif
#endif
#elif __MACH__
#include <cstdlib>
static void SimulateEscKeyPress()
{
    std::system("osascript -e 'tell application \"System Events\" to key code 53'");
}
#if defined PYTHON39
PYBIND11_MODULE(pencil4line_for_blender_mac_39, m)
#elif defined PYTHON310
PYBIND11_MODULE(pencil4line_for_blender_mac_310, m)
#elif defined PYTHON311
#ifndef NEW_PROPERTY_RNA
PYBIND11_MODULE(pencil4line_for_blender_mac_311, m)
#else
PYBIND11_MODULE(pencil4line_for_blender_mac_311_450, m)
#endif
#endif
#elif __linux__
#if defined PYTHON39
PYBIND11_MODULE(pencil4line_for_blender_linux_39, m)
#elif defined PYTHON310
PYBIND11_MODULE(pencil4line_for_blender_linux_310, m)
#elif defined PYTHON311
#ifndef NEW_PROPERTY_RNA
PYBIND11_MODULE(pencil4line_for_blender_linux_311, m)
#else
PYBIND11_MODULE(pencil4line_for_blender_linux_311_450, m)
#endif
#endif
#endif
{
	m.doc() = "Pencil+ 4 for Blender";
	m.def("get_commit_hash", [] { return GIT_COMMIT_HASH; });
	m.def("set_blender_version", &interm::BlenderVersion::Set);
	m.def("set_render_app_path", [](std::wstring& str)  {interm::Context::renderAppPath = str; });
    m.def("create_previews", [](
        int previewSize,
        int strokePreviewWidth,
        std::shared_ptr<Nodes::BrushDetailNodeToExport> brushDtailNode,
        float strokePreviewBrushSize,
        float strokePreviewScale,
        const std::array<float, 4> color,
        const std::array<float, 4> bgColor,
        std::shared_ptr<interm::DataHash> hashPrev)
    { return interm::CreatePreviews(previewSize, strokePreviewWidth, brushDtailNode, strokePreviewBrushSize, strokePreviewScale, color, bgColor, hashPrev); });
#ifdef __MACH__
    m.def("simulate_esc_key_press", &SimulateEscKeyPress);
#endif
    
	py::class_<interm::Camera>(m, "interm_camera")
		.def(py::init<float, float, int, std::vector<std::vector<float>>&, std::vector<std::vector<float>>&>())
		;
	py::class_<interm::RenderInstance>(m, "interm_render_Instance")
		.def(py::init<py::object, std::vector<std::vector<float>>&, py::object, bool, std::vector<py::object>&>())
		;
	py::class_<interm::CurveData>(m, "interm_curve_data")
		.def(py::init<std::vector<py::object>&, std::vector<int>>())
		;

	py::class_<interm::Context>(m, "interm_context")
		.def(py::init<>())
		.def("draw", [](interm::Context& self,
			py::object image,
			py::object cameraObject,
			std::vector<py::object> objects,
			py::object materialOverride,
			std::vector<std::pair<py::object, py::object>> curveData,
			std::vector<std::shared_ptr<Nodes::LineNodeToExport>> lineNodes,
			std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>> lineFunctions,
			std::vector<std::shared_ptr<Nodes::LineRenderElementToExport>> lineRenderElements,
			std::vector<std::shared_ptr<Nodes::VectorOutputToExport>> vectorOutputs,
			std::vector<std::vector<py::object>> groups)
			{ return self.Draw(blrna::Image(image), cameraObject.cast<interm::Camera>(), objects, materialOverride, curveData, lineNodes, lineFunctions, lineRenderElements, vectorOutputs, groups); })
		.def("draw_for_viewport", [](interm::Context& self,
			int width,
			int height,
			py::object cameraObject,
			std::vector<py::object> objects,
			py::object materialOverride,
			std::vector<std::pair<py::object, py::object>> curveData,
			std::vector<std::shared_ptr<Nodes::LineNodeToExport>> lineNodes,
			std::vector<std::shared_ptr<Nodes::LineFunctionsNodeToExport>> lineFunctions,
			std::vector<std::vector<py::object>> groups)
			{ return self.DrawForViewport(width, height, cameraObject.cast<interm::Camera>(), objects, materialOverride, curveData, lineNodes, lineFunctions, groups); })
		.def("cleanup_all", [](interm::Context& self) { self.CleanupAll(); })
		.def("cleanup_frame", [](interm::Context& self) { self.CleanupFrame(); })
		.def_readwrite("task_name", &interm::Context::taskName)
		.def_readwrite("platform", &interm::Context::platform)
		.def("get_viewport_image_buffer", [](interm::Context& self)
		{
			if (!self.GetViewportImageBuffer())
			{
				return py::array_t<float>(0, nullptr);
			}
			const auto& buff = *self.GetViewportImageBuffer();
			return py::array_t<float>(buff.size(), buff.data());
		})
		.def("clear_viewport_image_buffer", [](interm::Context& self) { self.ClearViewportImageBuffer(); })
		.def_readwrite("draw_options", &interm::Context::drawOptions)
		.def_readwrite("mesh_color_attributes_on", &interm::Context::mesh_color_attributes_on)
		.def_readwrite("mesh_color_attributes", &interm::Context::mesh_color_attributes)
		;

	py::enum_<RenderAppRet>(m, "draw_ret")
		.value("success", RenderAppRet::Success)
		.value("timeout", RenderAppRet::Timeout)
		.value("cancle", RenderAppRet::Cancel)
		.value("success_without_license", RenderAppRet::SuccessWithoutLicense)
		.value("error_unknown", RenderAppRet::Error_Unknown)
		.value("error_wait_for_another", RenderAppRet::Error_WaitForAnother)
		.value("error_not_in_session", RenderAppRet::Error_NotInSession)
		.value("error_invalid_params", RenderAppRet::Error_InvalidParams)
		.value("error_invalid_render_app_version", RenderAppRet::Error_InvalidRenderAppVersion)
		.value("error_access_render_app", RenderAppRet::Error_AccessRenderApp)
		.value("error_exec_render_app", RenderAppRet::Error_ExecRenderApp)
		;

	py::class_<interm::DrawOptions, std::shared_ptr<interm::DrawOptions>>(m, "draw_options")
		.def(py::init<>())
		.def_readwrite("timeout", &interm::DrawOptions::timeout)
		.def_readwrite("line_scale", &interm::DrawOptions::line_scale)
		.def_readwrite("linesize_relative_target_width", &interm::DrawOptions::linesize_relative_target_width)
		.def_readwrite("linesize_relative_target_height", &interm::DrawOptions::linesize_relative_target_height)
		.def_readwrite("linesize_absolute_scale", &interm::DrawOptions::linesize_absolute_scale)
		.def_readwrite("objects_cache_valid", &interm::DrawOptions::objects_cache_valid)
		;

	py::class_<interm::DataHash, std::shared_ptr<interm::DataHash>>(m, "data_hash")
		.def(py::init<>())
		;

	Nodes::registerEnums(m);
	Nodes::LineNodeToExport::registerClass(m);
	Nodes::LineSetNodeToExport::registerClass(m);
	Nodes::BrushSettingsNodeToExport::registerClass(m);
	Nodes::BrushDetailNodeToExport::registerClass(m);
	Nodes::ReductionSettingsNodeToExport::registerClass(m);
	Nodes::TextureMapNodeToExport::registerClass(m);
	Nodes::LineFunctionsNodeToExport::registerClass(m);
	Nodes::LineRenderElementToExport::registerClass(m);
	Nodes::VectorOutputToExport::registerClass(m);
}
