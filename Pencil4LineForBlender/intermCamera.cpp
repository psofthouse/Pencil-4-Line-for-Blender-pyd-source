#include "intermCamera.h"
#include "blrnaCamera.h"
#include "blrnaObject.h"

namespace interm
{
	Camera::Camera(float nearClip, float farClip, int sensorFit, const std::vector<std::vector<float>>& w, const std::vector<std::vector<float>>& p):
		_nearClip(nearClip),
		_farClip(farClip),
		_sensorFit(sensorFit),
		_matrixWorld{
			w[0][0], w[1][0], w[2][0], w[3][0],
			w[0][1], w[1][1], w[2][1], w[3][1],
			w[0][2], w[1][2], w[2][2], w[3][2],
			w[0][3], w[1][3], w[2][3], w[3][3],
		},
		_matrixProjection{
			p[0][0], p[1][0], p[2][0], p[3][0],
			p[0][1], p[1][1], p[2][1], p[3][1],
			p[0][2], p[1][2], p[2][2], p[3][2],
			p[0][3], p[1][3], p[2][3], p[3][3],
		}
	{}
}