#pragma once

#include <chaos/FileResource.h>
#include <chaos/FileTools.h>

namespace chaos
{
	void NamedResource::SetName(char const * in_name)
	{
		if (in_name != nullptr)
			name = in_name;
		else
			name.clear();
	}

	void FileResource::SetPath(boost::filesystem::path const & in_path)
	{
		boost::filesystem::path redirected_path;
		if (FileTools::GetRedirectedPath(in_path, redirected_path))
		{
			try
			{
				file_timestamp = boost::filesystem::last_write_time(redirected_path);
			}
			catch (boost::filesystem::filesystem_error &)
			{
				file_timestamp = boost::filesystem::last_write_time(in_path);
			}
		}
		else
			file_timestamp = boost::filesystem::last_write_time(in_path);
		path = in_path;
	}

}; // namespace chaos
