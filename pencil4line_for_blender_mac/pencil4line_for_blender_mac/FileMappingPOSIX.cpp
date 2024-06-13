#include <unistd.h>
#include <sys/stat.h>
#include "FileMappingPOSIX.hpp"

static int MemoryAllocationGranularity()
{
    static int g_memoryAllocationGranularity = -1;
    if (g_memoryAllocationGranularity < 0)
    {
        g_memoryAllocationGranularity = getpagesize();
    }

    return g_memoryAllocationGranularity;
}


FileMappingPOSIX::FileMappingPOSIX(const char* name, size_t bytes)
{
    _size = 0;
    _fd = ::shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
    if (_fd >= 0)
    {
        _alreadyExists  = true;
    }
    else
    {
        _alreadyExists = false;
        _fd = ::shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if (_fd >= 0)
        {
            if (::ftruncate(_fd, bytes) == -1)
            {
                ::shm_unlink(name);
                _fd = -1;
                return;
            }
            _size = bytes;
        }
    }
    
    _fdForRead = ::shm_open(name, O_RDONLY, S_IRUSR);
    
    if (_fd >= 0 && _fdForRead >= 0)
    {
        _name = name;
    }
}

FileMappingPOSIX::~FileMappingPOSIX()
{
    if (_fd >= 0)
    {
        if (_alreadyExists)
        {
            ::close(_fd);
        }
        else
        {
            ::shm_unlink(_name.data());
        }
    }
    if (_fdForRead >= 0)
    {
        ::close(_fdForRead);
    }
}

std::shared_ptr<FileMappingPOSIX::View> FileMappingPOSIX::CreateView(size_t offset, size_t bytes, int prot)
{
    auto fd = prot & PROT_WRITE ? _fd : _fdForRead;
    if (fd >= 0) {
        auto pView = new View(fd, offset, bytes, prot);
        if (pView && pView->GetPtr()) {
            return std::shared_ptr<FileMappingPOSIX::View>(pView);
        }
        delete pView;
    }
    return nullptr;
}

FileMappingPOSIX::View::View(int fd, size_t offset, size_t bytes, int prot)
{
    auto granularity = MemoryAllocationGranularity();
    auto margin = granularity > 0 ? (offset - granularity * (offset / granularity)) : 0;

    offset -= margin;
    bytes += margin;

    int flags = MAP_SHARED;//prot & PROT_WRITE ? MAP_SHARED : MAP_PRIVATE;
    _ptrOrg = ::mmap(0, bytes, prot, flags, fd, offset);
    if (_ptrOrg != MAP_FAILED)
    {
        _ptr = static_cast<char*>(_ptrOrg) + margin;
        _bytes = bytes;
        _prot = prot;
    }
    else
    {
        perror("mmap");
        _ptrOrg = nullptr;
    }
}

FileMappingPOSIX::View::~View()
{
    Flush();
    if (_ptrOrg) {
        ::munmap(_ptrOrg, _bytes);
    }
}

bool FileMappingPOSIX::View::Flush()
{
    return _prot & PROT_WRITE ? (_ptrOrg ? ::msync(_ptrOrg, _bytes, MS_SYNC) == 0 : false)
    : true;
}

