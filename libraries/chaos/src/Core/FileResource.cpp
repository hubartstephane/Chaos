#pragma once

#include <chaos/Chaos.h>

namespace chaos
{
	void FileResource::SetPath(boost::filesystem::path const & in_path)
	{
		bool filestamp_found = false;

#if _DEBUG // we cannot use 'CHAOS_CAN_REDIRECT_RESOURCE_FILES' inside libraries !!!
		boost::filesystem::path redirected_path;
		if (FileTools::GetRedirectedPath(in_path, redirected_path))
		{
			try
			{
				file_timestamp = boost::filesystem::last_write_time(redirected_path);
				filestamp_found = true;
			}
			catch (boost::filesystem::filesystem_error &)
			{
			}
		}
#endif // _DEBUG 

		if (!filestamp_found)
		{
			try
			{
				file_timestamp = boost::filesystem::last_write_time(in_path);
			}
			catch (boost::filesystem::filesystem_error &)
			{				
			}		
		}
		path = in_path;
	}

}; // namespace chaos
