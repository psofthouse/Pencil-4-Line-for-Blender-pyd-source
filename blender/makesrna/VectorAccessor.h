#pragma once

template <typename T, typename U=size_t, int64_t paddingSize=0> class VectorAccessor
{
private:
	T* begin_;
	T* end_;
	T* capacity_end_;

	U padding_[paddingSize];
public:
	inline size_t size() const { return end_ - begin_; }
	const T& operator[](size_t index) const { return begin_[index]; }
	T& operator[](size_t index) { return begin_[index]; }
};
