#pragma once

#ifdef _WINDOWS
#include <tchar.h>

constexpr const TCHAR* FILEMAPPING_HWND = _T("Pencil_4-RenderApp-HWND");
constexpr const TCHAR* FILEMAPPING_DATAFILE = _T("Pencil_4-RenderApp-Buffer");

constexpr const TCHAR* FILEMAPPING_SESSION_ID_PREFIX = _T("Pencil_4-RenderApp-SessionID-");

#define WM_SESSION_BIGIN			(WM_APP + 0)
#define WM_SESSION_DATAFILE_REQUEST	(WM_APP + 1)
#define WM_SESSION_RENDER			(WM_APP + 2)
#define WM_SESSION_END				(WM_APP + 3)
#endif

enum class RenderAppRet : int
{
	Error_ExecRenderApp = -1000,
	Error_AccessRenderApp,

	Error_InvalidRenderAppVersion = -100,
	Error_InvalidParams,
	Error_NotInSession,
	Error_WaitForAnother,

	Error_Unknown = -1,

	Success = 0,
	Timeout,
	Cancel,
	SuccessWithoutLicense,
};