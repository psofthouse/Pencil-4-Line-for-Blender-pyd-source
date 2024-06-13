#pragma once
#include <memory>
#include <string>
#include "RenderAppDefine.h"

namespace RenderApp
{
	class DataAccessor
	{
	public:
		enum DesiredAccess
		{
			Read,
			Write,
			ReadWrite,
		};

		DataAccessor() {}
		DataAccessor(const DataAccessor&) = delete;
		DataAccessor& operator=(const DataAccessor&) = delete;

		void* ptr() const { return _ptr; }
		template <class T> T ptr() const { return static_cast<T>(_ptr); }

	protected:
		void* _ptr;
	};

	class Session
	{
	public:
		static std::shared_ptr<Session> Create(const std::wstring& renderAppPath, double timeout = 0);
		static std::shared_ptr<Session> CreateForPreview(const std::wstring& renderAppPath);

		Session(const Session&) = delete;
		Session& operator=(const Session&) = delete;

		virtual int GetDataGranularity() const = 0;
		virtual bool RequestData(size_t bytes) = 0;
		virtual std::shared_ptr<DataAccessor> AccessData(size_t offset, size_t bytes, DataAccessor::DesiredAccess desiredAccess) = 0;
		virtual bool Render() = 0;

		bool IsReady() const { return _id; }
		RenderAppRet GetLastRenderAppRet() const { return _lastRenderAppRet; }

	protected:
		Session() {}

		size_t _id = 0;
		RenderAppRet _lastRenderAppRet = RenderAppRet::Success;
	};
}

