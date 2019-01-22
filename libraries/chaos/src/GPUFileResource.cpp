#include <chaos/GPUFileResource.h>

namespace chaos
{	
	void GPUFileResource::SetName(char const * in_name)
	{
		if (in_name != nullptr)
			name = in_name;
		else
			name.clear();
	}

	void GPUFileResource::SetPath(boost::filesystem::path const & in_path)
	{	
		file_timestamp = boost::filesystem::last_write_time(in_path);
		path = in_path;
	}

	void GPUFileResourceFriend::SetResourceName(GPUFileResource * resource, char const * in_name)
	{
		assert(resource != nullptr);
		resource->SetName(in_name);
	}

	void GPUFileResourceFriend::SetResourcePath(GPUFileResource * resource, boost::filesystem::path const & in_path)
	{
		assert(resource != nullptr);
		resource->SetPath(in_path);
	}

}; // namespace chaos
