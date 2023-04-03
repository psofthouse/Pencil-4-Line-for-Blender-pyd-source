#pragma once
#ifdef _WINDOWS
#include <functional>
#include <chrono>
#include "RenderAppSession.h"

class FileMapping;

namespace RenderApp
{
	class SessionWindows : public Session
	{
	friend class Session;
	public:
		~SessionWindows();

		int GetDataGranularity() const override { return _dataGranularity; }
		bool RequestData(size_t bytes) override;
		std::shared_ptr<DataAccessor> AccessData(size_t offset, size_t bytes, DataAccessor::DesiredAccess desiredAccess) override;
		bool Render() override;

	protected:
		SessionWindows(const std::wstring& renderAppPath, double timeout);

	private:
		std::shared_ptr<FileMapping> _fileMappingProc;
		std::shared_ptr<FileMapping> _fileMappingData;
		int _dataGranularity;
		bool _useTimeout;
		std::chrono::steady_clock::time_point _timeout_point;
		const std::wstring _renderAppPath;

		bool SendMessage(UINT message, LPARAM lParam = 0);
		void DoRenderAppHwndFunc(const std::function<void(HWND)>& func);
	};
}

#endif