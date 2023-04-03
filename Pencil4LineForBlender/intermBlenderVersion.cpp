#include "intermBlenderVersion.h"


namespace interm
{
	BlenderVersion BlenderVersion::g_shared;

	void BlenderVersion::Set(int major, int minor, int point)
	{
		g_shared._major = major;
		g_shared._minor = minor;
		g_shared._point = point;
	}

	bool BlenderVersion::operator>=(const BlenderVersion& other) const
	{
		if (_major > other._major) return true;
		if (_major == other._major)
		{
			if (_minor > other._minor) return true;
			if (_minor == other._minor)
			{
				if (_point >= other._point) return true;
			}
		}

		return false;
	}
}