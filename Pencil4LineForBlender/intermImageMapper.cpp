#include <algorithm>
#include "intermImageMapper.h"
#include "blrnaBase.h"
#include "blrnaImage.h"

namespace interm
{
	std::size_t ImageMapper::Hash::operator()(const py::object& key) const
	{
		return std::hash<void*>()(blrna::ConvertToRNAData(key));
	}
	bool ImageMapper::Pred::operator()(const py::object& x, const py::object& y) const
	{
		return blrna::ConvertToRNAData(x) == blrna::ConvertToRNAData(y);
	}
	
	ImageMapper::ImageMapper(size_t offset, size_t align):_offset(offset), _align(std::max(align, (size_t)1))
	{}

	size_t ImageMapper::GetDataSize(py::object image, int bytesPerPixel) const
	{
		blrna::Image rnaImage(image);
		auto w = rnaImage.Width();
		auto h = rnaImage.Height();

		return (size_t)w * h * bytesPerPixel;
	}

	size_t ImageMapper::GetLastPtrOffset() const
	{
		return _offset;
	}

	size_t ImageMapper::GetNextPtrOffset() const
	{
		return _align * ((_offset + _align - 1) / _align);
	}

	size_t ImageMapper::GetPtrOffset(py::object image, int bytesPerPixel)
	{
		auto itr = _ptrOffsets.find(image);
		if (itr == _ptrOffsets.end())
		{
			auto size = GetDataSize(image, bytesPerPixel);
			if (size == 0)
			{
				return 0;
			}

			_offset = GetNextPtrOffset();
			itr = _ptrOffsets.emplace(image, ImageDataInfo{ _offset, size }).first;
			_offset += size;
		}

		return itr->second.offset;
	}

	float srgb_to_linear_float(float x)
	{
		if (x <= 0.0f)			return 0.0f;
		else if (x < 0.04045f)	return x / 12.92f;
		else					return powf((x + 0.055f) / 1.055f, 2.4f);
	}

	static void ConvertColorSpace(float* pBuff, size_t numPixels, EnumPropertyItem* colorspace)
	{
		if (!pBuff || !colorspace)
		{
			return;
		}

		if (strcmp("sRGB", colorspace->name) == 0)
		{
			for (int i = 0; i < (int)numPixels; i++)
			{
				pBuff[i * 4 + 0] = srgb_to_linear_float(pBuff[i * 4 + 0]);
				pBuff[i * 4 + 1] = srgb_to_linear_float(pBuff[i * 4 + 1]);
				pBuff[i * 4 + 2] = srgb_to_linear_float(pBuff[i * 4 + 2]);
			}
		}
	}

	bool ImageMapper::WriteImageData(std::function<void* (size_t, size_t)> getPtrFunc,
		std::function<void (size_t, size_t)> postWriteFunc,
		std::vector<float>& workBuffer) const
	{
		for (const auto& item: _ptrOffsets)
		{
			auto size = item.second.bytes;
			if (size == 0)
			{
				return false;
			}

			auto ptr = getPtrFunc(item.second.offset, size);
			if (ptr == nullptr)
			{
				return false;
			}

			blrna::Image rnaImage(item.first);
			auto w = rnaImage.Width();
			auto h = rnaImage.Height();

			blrna::ColorManagedInputColorspaceSettings colorspaceSettings(rnaImage.get_colorspace_settings());
			auto colorspace = colorspaceSettings.get_name_item();

			if (size == (size_t)w * h * 16)
			{
				// RGBAfloat
				rnaImage.get_pixels(static_cast<float*>(ptr));
				ConvertColorSpace(static_cast<float*>(ptr), (size_t)w * h, colorspace);
			}
			else if (size == (size_t)w * h * 4)
			{
				// RGBA32
				auto* pDst = static_cast<unsigned char*>(ptr);

				workBuffer.resize((size_t)w * h * 4);
				auto* pSrc = workBuffer.data();
				rnaImage.get_pixels(pSrc);
				ConvertColorSpace(pSrc, (size_t)w * h, colorspace);

				for (int i = 0; i < size; i++)
				{
					auto v = (int)(pSrc[i] * 255 + 0.5f);
					pDst[i] = (unsigned char)(std::min(std::max(v, 0), 255));
				}
			}
			else
			{
				return false;
			}

			postWriteFunc(item.second.offset, size);
		}

		return true;
	}

}