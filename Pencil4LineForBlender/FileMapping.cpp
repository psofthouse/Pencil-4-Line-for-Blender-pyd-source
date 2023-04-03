#ifdef _WINDOWS
#include <intsafe.h>
#include "FileMapping.h"

static int MemoryAllocationGranularity()
{
	static int g_memoryAllocationGranularity = -1;
	if (g_memoryAllocationGranularity < 0)
	{
		SYSTEM_INFO info;
		GetSystemInfo(&info);

		g_memoryAllocationGranularity = info.dwAllocationGranularity;
	}

	return g_memoryAllocationGranularity;
}


FileMapping::FileMapping(LPCTSTR name, size_t bytes)
{
	_hFileMapping = ::CreateFileMapping(
		INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, HIDWORD(bytes), LODWORD(bytes), name
	);
	if (_hFileMapping)
	{
		_alreadyExists = GetLastError() == ERROR_ALREADY_EXISTS;
		_size = _alreadyExists ? 0 : bytes;
	}
	else
	{
		_alreadyExists = false;
		_size = 0;
	}
}

FileMapping::~FileMapping()
{
	if (_hFileMapping) {
		::CloseHandle(_hFileMapping);
	}
}

std::shared_ptr<FileMapping::View> FileMapping::CreateView(size_t offset, size_t bytes, DWORD desiredAccess)
{
	if (_hFileMapping) {
		auto pView = new View(_hFileMapping, offset, bytes, desiredAccess);
		if (pView && pView->GetPtr()) {
			return std::shared_ptr<FileMapping::View>(pView);
		}
	}
	return nullptr;
}


FileMapping::View::View(HANDLE hFileMapping, size_t offset, size_t bytes, DWORD desiredAccess)	
{
	auto granularity = MemoryAllocationGranularity();
	auto margin = granularity > 0 ? (offset - granularity * (offset / granularity)) : 0;

	offset -= margin;
	bytes += margin;

	_ptrOrg = ::MapViewOfFile(hFileMapping, desiredAccess, HIDWORD(offset), LODWORD(offset), bytes);
	if (_ptrOrg)
	{
		_ptr = static_cast<char*>(_ptrOrg) + margin;
	}
}

FileMapping::View::~View()
{
	Flush();
	if (_ptrOrg) {
		::UnmapViewOfFile(_ptrOrg);
	}
}

bool FileMapping::View::Flush()
{
	return _ptrOrg ? ::FlushViewOfFile(_ptrOrg, 0) : false;
}

#endif