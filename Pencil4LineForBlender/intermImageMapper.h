#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include "pybind11/pybind11.h"
namespace py = pybind11;

namespace interm
{
	class ImageMapper
	{
	public:
		ImageMapper(size_t offset = 0, size_t align = 1);
		size_t GetPtrOffset(py::object image, int bytesPerPixel = 4);
		size_t GetLastPtrOffset() const;
		size_t GetNextPtrOffset() const;

		bool WriteImageData(std::function<void*(size_t, size_t)> getPtrFunc,
			std::function<void (size_t, size_t)> postWriteFunc,
			std::vector<float>& workBuffer) const;

	private:
		size_t GetDataSize(py::object image, int bytesPerPixel) const;

		size_t _offset;
		const size_t _align;

		struct Hash { std::size_t operator()(const py::object& key) const; };
		struct Pred { bool operator()(const py::object& x, const py::object& y) const; };


		struct ImageDataInfo
		{
			size_t offset;
			size_t bytes;
		};
		std::unordered_map<py::object, ImageDataInfo, Hash, Pred> _ptrOffsets;
	};
}
