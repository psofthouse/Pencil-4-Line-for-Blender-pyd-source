#pragma once
#include <string>
#include <memory>
#include <fcntl.h>
#include <sys/mman.h>

class FileMappingPOSIX final
{
public:
    class View
    {
        friend class FileMappingPOSIX;

    protected:
        View(int fd, size_t offset, size_t bytes, int prot);

    public:
        View(const View&) = delete;
        View& operator=(const View&) = delete;

        ~View();

        void* GetPtr() const { return _ptr; }
        template<class T> T GetPtr() const { return static_cast<T>(_ptr); }

        bool Flush();

    private:
        void* _ptr = nullptr;
        void* _ptrOrg = nullptr;
        size_t _bytes = 0;
        int _prot = 0;
    };

    FileMappingPOSIX(const char* name, size_t bytes);
    ~FileMappingPOSIX();

    FileMappingPOSIX(const FileMappingPOSIX&) = delete;
    FileMappingPOSIX& operator=(const FileMappingPOSIX&) = delete;

    std::shared_ptr<View> CreateView(size_t offset, size_t bytes, int prot = PROT_READ | PROT_WRITE);

    bool IsAlreadyExists() const { return _alreadyExists; }
    size_t Size() const { return _size; }

private:
    int _fd = -1;
    int _fdForRead = -1;
    bool _alreadyExists;
    size_t _size = 0;
    std::string _name;
};
