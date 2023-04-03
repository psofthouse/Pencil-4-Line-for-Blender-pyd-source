#pragma once

namespace interm
{
	struct DrawOptions
	{
		double timeout = 0;
		float line_scale = 1.0f;
		float linesize_relative_target_width = 0;
		float linesize_relative_target_height = 0;
		bool objects_cache_valid = false;
	};
}

