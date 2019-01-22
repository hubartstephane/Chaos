#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{
	class GPUFileResource : public GPUResource
	{
		friend class GPUFileResourceFriend;

	public:

		/** get the name of the object */
		char const * GetName() const { return name.c_str(); }
		/** get the path of the object */
		boost::filesystem::path const & GetPath() const { return path; }

	protected:

		/** Set the name method (for friends only) */
		void SetName(char const * in_name);
		/** Set the path method (for friends only) */
		void SetPath(boost::filesystem::path const & in_path);

	protected:

		/** the name of the object */
		std::string name;
		/** the path of the object */
		boost::filesystem::path path;
		/** the file timestamp */
		std::time_t file_timestamp = 0;
	};

	class GPUFileResourceFriend
	{
	protected:

		/** Set the name method (for friends only) */
		static void SetResourceName(GPUFileResource * resource, char const * in_name);
		/** Set the path method (for friends only) */
		static void SetResourcePath(GPUFileResource * resource, boost::filesystem::path const & in_path);
	};

}; // namespace chaos
