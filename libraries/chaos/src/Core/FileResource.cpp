#pragma once

#include <chaos/Chaos.h>

namespace chaos
{
	void FileResource::SetPath(boost::filesystem::path const & in_path)
	{
		FileTools::ForEachRedirectedPath(in_path, [this](boost::filesystem::path const& p)
		{
			try
			{
				file_timestamp = boost::filesystem::last_write_time(p);
				return true;
			}
			catch (boost::filesystem::filesystem_error &)
			{
			}
			return false;
		});
		path = in_path;
	}

}; // namespace chaos
