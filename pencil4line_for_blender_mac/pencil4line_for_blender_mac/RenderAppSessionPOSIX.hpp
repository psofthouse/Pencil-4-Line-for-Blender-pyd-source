#pragma once
#include <functional>
#include <chrono>
#include "RenderAppSession.h"

class FileMappingPOSIX;

namespace RenderApp
{
    class MessageSenderPOSIX;

    class SessionPOSIX : public Session
    {
    friend class Session;
    public:
        ~SessionPOSIX();

        int GetDataGranularity() const override { return _dataGranularity; }
        bool RequestData(size_t bytes) override;
        std::shared_ptr<DataAccessor> AccessData(size_t offset, size_t bytes, DataAccessor::DesiredAccess desiredAccess) override;
        bool Render() override;

    protected:
        SessionPOSIX(const std::wstring& renderAppPath, double timeout, bool forPreview);

    private:
        std::shared_ptr<FileMappingPOSIX> _fileMappingData;
        std::shared_ptr<MessageSenderPOSIX> _messageSender;
        
        int _dataGranularity;
        const std::wstring _renderAppPath;

        const char* _filemapping_datafile;
    };
}

