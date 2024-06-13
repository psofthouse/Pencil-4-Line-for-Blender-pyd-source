#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <limits.h>
#include "RenderAppDefine.h"
#include "FileMappingPOSIX.hpp"
#include "RenderAppSessionPOSIX.hpp"

namespace RenderApp
{
static bool CanProceed(RenderAppRet renderAppRet)
{
    return (int)renderAppRet >= 0 && renderAppRet != RenderAppRet::Timeout && renderAppRet != RenderAppRet::Cancel;
}


static std::string toAStr(const wchar_t* wstr)
{
    auto wstr_len = wcslen(wstr);
    auto lc_org = setlocale(LC_CTYPE, NULL);
    setlocale(LC_CTYPE, "");

    size_t buff_len = wcstombs(nullptr, wstr, wstr_len);
    if (buff_len != -1)
    {
        auto buff = std::make_unique<char[]>(buff_len);
        auto len = wcstombs(buff.get(), wstr, buff_len);
        if (len > 0)
        {
            auto temp = setlocale(LC_CTYPE, lc_org);
            return std::string(buff.get(), len);
        }
    }
    setlocale(LC_CTYPE, lc_org);
    return std::string();
}

static std::string toAStr(const std::wstring& wstr)
{
    return toAStr(wstr.c_str());
}


/// <summary>
/// 
/// </summary>
/// <param name="renderAppPath"></param>
/// <returns>新規にRenderAppを起動させた場合はture</returns>
static bool StartRenderApp(const std::string renderAppPath)
{
    if (renderAppPath.empty()) {
        return false;
    }

    if (access(renderAppPath.c_str(), X_OK) != 0) {
        return false;
    }

#ifdef __MACH__
    std::string cmd2 = "open --background \"" + renderAppPath + "\"";
#else
    std::string cmd2 = renderAppPath + " &"; // バックグラウンド起動する
#endif
    system(cmd2.c_str());
    return true;
}

static bool StartRenderApp(const std::wstring& renderAppPath)
{
    return StartRenderApp(toAStr(renderAppPath));
}


class DataAccessorPOSIX : public DataAccessor
{
public:
    DataAccessorPOSIX(std::shared_ptr<FileMappingPOSIX> fileMapping, size_t offset, size_t bytes, DesiredAccess desiredAccess):DataAccessor()
    {
        int prot = PROT_READ | PROT_WRITE;
        switch (desiredAccess)
        {
        case RenderApp::DataAccessor::Read:
            prot = PROT_READ;
            break;
        case RenderApp::DataAccessor::Write:
            prot = PROT_WRITE;
            break;
        default:
            break;
        }

        _mapView = fileMapping->CreateView(offset, bytes, prot);
        _ptr = _mapView->GetPtr();
    }
private:
    std::shared_ptr<FileMappingPOSIX::View> _mapView;
};


class MessageSenderPOSIX
{
public:
    MessageSenderPOSIX(RenderAppRet& lastRenderAppRet, double timeout, const std::wstring& renderAppPath, bool forPreview):_lastRenderAppRet(lastRenderAppRet),
    _sock_path(forPreview ? SOCK_PATH_FOR_PREVIEW : SOCK_PATH)
    {
        _useTimeout = timeout > 0;
        if (_useTimeout)
        {
            _timeout_point = std::chrono::steady_clock::now();
            _timeout_point += std::chrono::microseconds((int)(timeout * 1000 * 1000));
        }

        //
        struct sockaddr_un addr;

        _sockConnected = false;
        _sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (_sockfd == -1) {
            perror("socket");
            _lastRenderAppRet = RenderAppRet::Error_AccessRenderApp;
            return;
        }
        
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, _sock_path, sizeof(addr.sun_path) - 1);
        
        for (int i = 0; true; ++i) {
            if (connect(_sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
                if (renderAppPath.empty()) {
                    return;
                }
                if (i == 3) {
                    perror("connect");
                    _lastRenderAppRet = RenderAppRet::Error_AccessRenderApp;
                    return;
                }
                StartRenderApp(renderAppPath);
                usleep(100 * 1000);
            }
            else
            {
                _sockConnected = true;
                return;
            }
        }
    }
    
    ~MessageSenderPOSIX()
    {
        if (_sockfd >= 0)
        {
            RenderAppMessage msg;
            msg.msgid = RenderAppMessageID::END;
            msg.arg = 0;
            ::send(_sockfd, &msg, sizeof(msg), 0);

            ::close(_sockfd);
        }
    }
    
    bool Send(RenderAppMessageID msgid, int64_t arg = 0)
    {
        if (_sockfd == -1 || !_sockConnected) {
            _lastRenderAppRet = RenderAppRet::Error_Unknown;
            return false;
        }
        
        struct timespec timeout;
        if (_useTimeout)
        {
            auto duratinon = std::chrono::duration_cast<std::chrono::microseconds>(_timeout_point - std::chrono::steady_clock::now()).count();
            if (duratinon <= 0)
            {
                _lastRenderAppRet = RenderAppRet::Timeout;
                return false;
            }
            timeout.tv_sec = duratinon / (1000 * 1000);
            timeout.tv_nsec = (duratinon - timeout.tv_sec * 1000 * 1000) * 1000;
        }
        else
        {
            timeout.tv_sec = 1;
            timeout.tv_nsec = 0;
        }
        
        RenderAppMessage msg;
        msg.msgid = msgid;
        msg.arg = arg;
        
        auto result = ::send(_sockfd, &msg, sizeof(msg), 0);
        if (result < 0) {
            perror("send");
            _lastRenderAppRet = RenderAppRet::Error_Unknown;
            return false;
        }
        
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(_sockfd, &readfds);
        while (true) {
            fd_set tmp_readfds = readfds;
            int ret = pselect(_sockfd + 1, &tmp_readfds, NULL, NULL, &timeout, NULL); // 通常のselectだとLinuxではtimeoutの中身が更新されてしまうのでpselect
            if (ret < 0) {
                // エラーが発生した場合の処理
                perror("select");
                break;
            } else if (ret == 0) {
                // タイムアウトした場合の処理
                if (_useTimeout)
                {
                    _lastRenderAppRet = RenderAppRet::Timeout;
                    return false;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                while (true)
                {
                    // データが到着した場合の処理
                    if (FD_ISSET(_sockfd, &tmp_readfds)) {
                        auto len = recv(_sockfd, &msg, sizeof(msg), 0);
                        if (len <= 0) {
                            // エラーが発生した/切断された場合の処理
                            perror("recv");
                            _lastRenderAppRet = RenderAppRet::Error_Unknown;
                            break;
                        } else if (msg.arg != 0) {
                            // 接続チェック
                            auto duratinon = std::chrono::duration_cast<std::chrono::milliseconds>(_timeout_point - std::chrono::steady_clock::now()).count();
                            if (_useTimeout && duratinon <= 0)
                            {
                                _lastRenderAppRet = RenderAppRet::Timeout;
                                break;
                            }
                            continue;
                        } else {
                            // データを受信した場合の処理
                            _lastRenderAppRet = (RenderAppRet)msg.ret;
                            break;
                        }
                    }
                }
                
                break;
            }
        }
        
        return CanProceed(_lastRenderAppRet);
    }
    
private:
    RenderAppRet& _lastRenderAppRet;
    int _sockfd = -1;
    
    bool _sockConnected = false;
    bool _useTimeout;
    std::chrono::steady_clock::time_point _timeout_point;
    
    const char* _sock_path;
};



std::shared_ptr<Session> Session::Create(const std::wstring& renderAppPath, double timeout)
{
    return std::shared_ptr<SessionPOSIX>(new SessionPOSIX(renderAppPath, timeout, false));
}

std::shared_ptr<Session> Session::CreateForPreview(const std::wstring& renderAppPath)
{
    return std::shared_ptr<SessionPOSIX>(new SessionPOSIX(renderAppPath, 0.100, true));
}

SessionPOSIX::SessionPOSIX(const std::wstring& renderAppPath, double timeout, bool forPreview):
_renderAppPath(renderAppPath),
_filemapping_datafile(forPreview ? FILEMAPPING_DATAFILE_FOR_PREVIEW : FILEMAPPING_DATAFILE)
{
    while (_id == 0)
    {
        auto t = std::chrono::steady_clock::now();
        _id = std::hash<double>{}((double)std::chrono::duration_cast<std::chrono::microseconds>(t.time_since_epoch()).count());
        _id ^= std::hash<void*>()(this);
    }
    
    //        while (true)
    //        {
    //            _lastRenderAppRet = RenderAppRet::Success;
    //            SendMessage(WM_SESSION_BIGIN, 0);
    //            if (_lastRenderAppRet != RenderAppRet::Error_WaitForAnother)
    //            {
    //                break;
    //            }
    //            Sleep(10);
    //        }

    _lastRenderAppRet = RenderAppRet::Success;
    _messageSender = std::make_shared<MessageSenderPOSIX>(_lastRenderAppRet, timeout, renderAppPath, forPreview);
    _messageSender->Send(RenderAppMessageID::BIGIN);
    _dataGranularity = (int)_lastRenderAppRet;
}

SessionPOSIX::~SessionPOSIX()
{
    _messageSender = nullptr;
    //        DoRenderAppHwndFunc([&](HWND hwnd)
    //        {
    //            ::SendMessageTimeout(hwnd, WM_SESSION_END, _id, 0, SMTO_NORMAL, 1, nullptr);
    //        });
}

bool SessionPOSIX::RequestData(size_t bytes)
{
    if (_messageSender->Send(RenderAppMessageID::DATAFILE_REQUEST, bytes))
    {
        _fileMappingData = std::make_shared<FileMappingPOSIX>(_filemapping_datafile, 1);
        if (_fileMappingData && _fileMappingData->IsAlreadyExists())
        {
            return true;
        }

        _fileMappingData = nullptr;
        _lastRenderAppRet = RenderAppRet::Error_Unknown;
    }
    
    return false;
}

std::shared_ptr<DataAccessor> SessionPOSIX::AccessData(size_t offset, size_t bytes, DataAccessor::DesiredAccess desiredAccess)
{
    if (_fileMappingData)
    {
        auto ret = std::make_shared<DataAccessorPOSIX>(_fileMappingData, offset, bytes, desiredAccess);
        if (ret && ret->ptr() != 0)
        {
            return ret;
        }
    }

    _lastRenderAppRet = RenderAppRet::Error_Unknown;
    return nullptr;
}

bool SessionPOSIX::Render()
{
    if (!_fileMappingData)
    {
        _lastRenderAppRet = RenderAppRet::Error_Unknown;
        return false;
    }
    
    _messageSender->Send(RenderAppMessageID::RENDER, 0);
    return _lastRenderAppRet == RenderAppRet::Success || _lastRenderAppRet == RenderAppRet::SuccessWithoutLicense;
}


}
