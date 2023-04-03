#pragma once
#ifdef _WINDOWS

#include <Windows.h>
#include <memory>

class FileMapping final
{
public:
	class View
	{
		friend class FileMapping;

	protected:
		View(HANDLE hFileMapping, size_t offset, size_t bytes, DWORD desiredAccess);

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
	};

	FileMapping(LPCTSTR name, size_t bytes);
	~FileMapping();

	FileMapping(const FileMapping&) = delete;
	FileMapping& operator=(const FileMapping&) = delete;

	std::shared_ptr<View> CreateView(size_t offset, size_t bytes, DWORD desiredAccess = FILE_MAP_ALL_ACCESS);

	bool IsAlreadyExists() const { return _alreadyExists; }
	size_t Size() const { return _size; }

private:
	HANDLE _hFileMapping = nullptr;
	bool _alreadyExists;
	size_t _size;
};

#endif