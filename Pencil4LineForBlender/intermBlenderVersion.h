#pragma once

namespace interm
{
	class BlenderVersion
	{
	public:
		BlenderVersion(int major, int minor, int point) : _major(major), _minor(minor), _point(point) {}
		BlenderVersion(int major, int minor) : _major(major), _minor(minor), _point(0) {}
		BlenderVersion(int major) : _major(major), _minor(0), _point(0) {}
		BlenderVersion() : _major(0), _minor(0), _point(0) {}

		static void Set(int major, int minor, int point);
		static const BlenderVersion& Shared() { return g_shared; }

		bool operator>=(const BlenderVersion& other) const;

	private:
		int _major;
		int _minor;
		int _point;

		static BlenderVersion g_shared;
	};

}


