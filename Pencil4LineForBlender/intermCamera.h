#pragma once
#include "pybind11/pybind11.h"
namespace py = pybind11;

namespace interm
{
	class Camera final
	{
	public:
		Camera(float nearClip,
			float farClip,
			int sensorFit,
			const std::vector<std::vector<float>>& worldMatrix,
			const std::vector<std::vector<float>>& projectionMatrix);

		float GetNearClip() const	{ return _nearClip; }
		float GetFarClip() const { return _farClip; }
		int GetSensorFit() const { return _sensorFit; }
		const float* GetMatrixWorld() const { return _matrixWorld; }
		const float* GetProjection() const { return _matrixProjection; }

	private:
		const float _nearClip;
		const float _farClip;
		const int _sensorFit;
		const float _matrixWorld[16];
		const float _matrixProjection[16];
	};

}
