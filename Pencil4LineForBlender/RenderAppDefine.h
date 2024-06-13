#pragma once

#ifdef _WINDOWS
#include <tchar.h>

constexpr const TCHAR* FILEMAPPING_HWND = _T("Pencil_4-RenderApp-HWND");
constexpr const TCHAR* FILEMAPPING_DATAFILE = _T("Pencil_4-RenderApp-Buffer");
constexpr const TCHAR* FILEMAPPING_HWND_FOR_PREVIEW = _T("Pencil_4-RenderApp-HWND-for-Preview");
constexpr const TCHAR* FILEMAPPING_DATAFILE_FOR_PREVIEW = _T("Pencil_4-RenderApp-Buffer-for-Preview");

constexpr const TCHAR* FILEMAPPING_SESSION_ID_PREFIX = _T("Pencil_4-RenderApp-SessionID-");

#define WM_SESSION_BIGIN			(WM_APP + 0)
#define WM_SESSION_DATAFILE_REQUEST	(WM_APP + 1)
#define WM_SESSION_RENDER			(WM_APP + 2)
#define WM_SESSION_END				(WM_APP + 3)
#elif defined(__MACH__) || defined(__linux__)
//constexpr const char* FILEMAPPING_SESSION_ID_PREFIX = "/PRA4-";
constexpr const char* FILEMAPPING_DATAFILE = "/Pencil_4-RenderApp-Buffer";
constexpr const char* SOCK_PATH = "/tmp/pcl4rend";
constexpr const char* FILEMAPPING_DATAFILE_FOR_PREVIEW = "/Pencil_4-RenderApp-Buffer2";
constexpr const char* SOCK_PATH_FOR_PREVIEW = "/tmp/pcl4rend2";
struct RenderAppMessage
{
    union
    {
        struct
        {
            int64_t msgid;
            int64_t arg;
        };
        int64_t ret;
    };
};

enum RenderAppMessageID
{
    BIGIN = 0,
    DATAFILE_REQUEST,
    RENDER,
    END,
};
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
