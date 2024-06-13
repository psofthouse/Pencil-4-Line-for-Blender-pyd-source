#ifdef _WINDOWS
#include <tchar.h>
#include <sstream>
#include <filesystem>
#include "FileMapping.h"
#include "RenderAppDefine.h"
#include "RenderAppSessionWindows.h"

namespace RenderApp
{
	class DataAccessorWindows : public DataAccessor
	{
	public:
		DataAccessorWindows(std::shared_ptr<FileMapping> fileMapping, size_t offset, size_t bytes, DesiredAccess desiredAccess):DataAccessor()
		{
			DWORD da = FILE_MAP_ALL_ACCESS;
			switch (desiredAccess)
			{
			case RenderApp::DataAccessor::Read:
				da = FILE_MAP_READ;
				break;
			case RenderApp::DataAccessor::Write:
				da = FILE_MAP_WRITE;
				break;
			default:
				break;
			}

			_mapView = fileMapping->CreateView(offset, bytes, da);
			_ptr = _mapView->GetPtr();
		}
	private:
		std::shared_ptr<FileMapping::View> _mapView;
	};


	std::shared_ptr<Session> Session::Create(const std::wstring& renderAppPath, double timeout)
	{
		return std::shared_ptr<Session>(new SessionWindows(renderAppPath, timeout, FILEMAPPING_HWND, FILEMAPPING_DATAFILE));
	}

	std::shared_ptr<Session> Session::CreateForPreview(const std::wstring& renderAppPath)
	{
		return std::shared_ptr<Session>(new SessionWindows(renderAppPath, 0, FILEMAPPING_HWND_FOR_PREVIEW, FILEMAPPING_DATAFILE_FOR_PREVIEW));
	}

	SessionWindows::SessionWindows(const std::wstring& renderAppPath, double timeout, const TCHAR* filemapping_hwnd, const TCHAR* filemapping_datafile):
		_renderAppPath(renderAppPath), _filemapping_hwnd_name(filemapping_hwnd), _filemapping_datafile_name(filemapping_datafile)
	{
		_useTimeout = timeout > 0;
		if (_useTimeout)
		{
			auto t = std::chrono::steady_clock::now();
			
			_timeout_point = std::chrono::steady_clock::now();
			_timeout_point += std::chrono::microseconds((int)(timeout * 1000 * 1000));
		}

		while (_id == 0)
		{
			auto t = std::chrono::steady_clock::now();
			_id = std::hash<double>{}((double)std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count());
			_id ^= std::hash<void*>()(this);
		}

		std::wostringstream oss;
		oss << FILEMAPPING_SESSION_ID_PREFIX << _id;
		_fileMappingProc = std::make_shared<FileMapping>(oss.str().data(), 8);

		while (true)
		{
			_lastRenderAppRet = RenderAppRet::Success;
			SendMessage(WM_SESSION_BIGIN, 0);
			if (_lastRenderAppRet != RenderAppRet::Error_WaitForAnother)
			{
				break;
			}
			Sleep(10);
		} 
	}

	SessionWindows::~SessionWindows()
	{
		_fileMappingProc = nullptr;	// レンダリングキャンセルのために最初に開放する
		DoRenderAppHwndFunc([&](HWND hwnd)
		{
			::SendMessageTimeout(hwnd, WM_SESSION_END, _id, 0, SMTO_NORMAL, 1, nullptr);
		});
	}

	static bool CanProceed(RenderAppRet renderAppRet)
	{
		return (int)renderAppRet >= 0 && renderAppRet != RenderAppRet::Timeout && renderAppRet != RenderAppRet::Cancel;
	}

	bool SessionWindows::SendMessage(UINT message, LPARAM lParam)
	{
		if (!IsReady())
		{
			_lastRenderAppRet = RenderAppRet::Error_Unknown;
			return false;
		}
		if (!CanProceed(_lastRenderAppRet))
		{
			return false;
		}

		DoRenderAppHwndFunc([&](HWND hwnd)
		{
			if (_useTimeout)
			{
				auto duratinon = std::chrono::duration_cast<std::chrono::milliseconds>(_timeout_point - std::chrono::steady_clock::now()).count();
				if (duratinon <= 0)
				{
					_lastRenderAppRet = RenderAppRet::Timeout;
				}
				else
				{
					DWORD_PTR dwRc = -1;
					auto ret = ::SendMessageTimeout(hwnd, message, _id, lParam, SMTO_NORMAL, (UINT)duratinon, &dwRc);
					if (ret > 0)
					{
						_lastRenderAppRet = static_cast<RenderAppRet>(dwRc);
					}
					else
					{
						_lastRenderAppRet = GetLastError() == ERROR_TIMEOUT ? RenderAppRet::Timeout :
							GetLastError() == ERROR_ACCESS_DENIED ? RenderAppRet::Error_AccessRenderApp : RenderAppRet::Error_Unknown;
					}
				}
			}
			else
			{
				::SetLastError(0);
				auto ret = ::SendMessage(hwnd, message, _id, lParam);
				_lastRenderAppRet = GetLastError() == ERROR_ACCESS_DENIED ? RenderAppRet::Error_AccessRenderApp : static_cast<RenderAppRet>(ret);
			}
		});

		if (message == WM_SESSION_BIGIN && (int)_lastRenderAppRet >= 0)
		{
			_dataGranularity = (int)_lastRenderAppRet;
			_lastRenderAppRet = RenderAppRet::Success;
		}

		return CanProceed(_lastRenderAppRet);
	}

	void SessionWindows::DoRenderAppHwndFunc(const std::function<void(HWND)>& func)
	{
		if (!IsReady())
		{
			_lastRenderAppRet = RenderAppRet::Error_Unknown;
			return;
		}

		std::shared_ptr<FileMapping> hwndFileMapping;
		for (int i = 0; !hwndFileMapping; i++)
		{
			hwndFileMapping = std::make_shared<FileMapping>(_filemapping_hwnd_name.data(), sizeof(HWND));
			if (!hwndFileMapping->IsAlreadyExists())
			{
				hwndFileMapping = nullptr;
				if (_renderAppPath.empty() || i == 3)
				{
					break;
				}
				auto exec_ret = ::ShellExecute(NULL, _T("open"), _renderAppPath.data(), NULL,
					std::filesystem::path(_renderAppPath).wstring().data(), SW_SHOWNORMAL);
				if ((INT_PTR)exec_ret <= 32)
				{
					break;
				}
				Sleep(100);
			}
		}
		if (!hwndFileMapping)
		{
			_lastRenderAppRet = RenderAppRet::Error_ExecRenderApp;
			return;
		}

		HWND hwnd = 0;
		while (true)
		{
			auto mapView = hwndFileMapping->CreateView(0, sizeof(HWND), FILE_MAP_READ);
			if (!mapView)
			{
				_lastRenderAppRet = RenderAppRet::Error_Unknown;
				return;
			}
			hwnd = *(HWND*)mapView->GetPtr();
			if (hwnd)
			{
				break;
			}

			if (_useTimeout)
			{
				if (_timeout_point < std::chrono::steady_clock::now())
				{
					_lastRenderAppRet = RenderAppRet::Error_ExecRenderApp;
					return;
				}
			}
			else
			{
				hwndFileMapping = nullptr;
				hwndFileMapping = std::make_shared<FileMapping>(_filemapping_hwnd_name.data(), sizeof(HWND));
				if (!hwndFileMapping->IsAlreadyExists())
				{
					_lastRenderAppRet = RenderAppRet::Error_ExecRenderApp;
					return;
				}
			}
			Sleep(100);
		}

		func(hwnd);
	}

	bool SessionWindows::RequestData(size_t bytes)
	{
		if (SendMessage(WM_SESSION_DATAFILE_REQUEST, bytes))
		{
			_fileMappingData = std::make_shared<FileMapping>(_filemapping_datafile_name.data(), 1);
			if (_fileMappingData && _fileMappingData->IsAlreadyExists())
			{
				return true;
			}

			_fileMappingData = nullptr;
		}

		return false;
	}

	std::shared_ptr<DataAccessor> SessionWindows::AccessData(size_t offset, size_t bytes, DataAccessor::DesiredAccess desiredAccess)
	{
		if (_fileMappingData)
		{
			auto ret = std::make_shared<DataAccessorWindows>(_fileMappingData, offset, bytes, desiredAccess);
			return ret && ret->ptr() != 0 ? ret : nullptr;
		}

		return nullptr;
	}

	bool SessionWindows::Render()
	{
		if (!_fileMappingData)
		{
			return false;
		}

		SendMessage(WM_SESSION_RENDER, ::GetCurrentProcessId());
		return _lastRenderAppRet == RenderAppRet::Success || _lastRenderAppRet == RenderAppRet::SuccessWithoutLicense;
	}
}

#endif