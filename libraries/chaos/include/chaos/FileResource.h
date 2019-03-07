#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	template<typename PARENT_CLASS>
	class FileResource : public PARENT_CLASS
	{

		friend class FileResourceFriend;

	public:

		/** get the name of the object */
		char const * GetName() const { return name.c_str(); }
		/** get the path of the object */
		boost::filesystem::path const & GetPath() const { return path; }

	protected:

		/** Set the name method (for friends only) */
		void SetName(char const * in_name)
		{
			if (in_name != nullptr)
				name = in_name;
			else
				name.clear();
		}
		/** Set the path method (for friends only) */
		void SetPath(boost::filesystem::path const & in_path)
		{
			file_timestamp = boost::filesystem::last_write_time(in_path);
			path = in_path;
		}

	protected:

		/** the name of the object */
		std::string name;
		/** the path of the object */
		boost::filesystem::path path;
		/** the file timestamp */
		std::time_t file_timestamp = 0;
	};

	class FileResourceFriend
	{
	protected:

		/** Set the name method (for friends only) */
		template<typename RESOURCE_TYPE>
		static void SetResourceName(RESOURCE_TYPE * resource, char const * in_name)
		{
			assert(resource != nullptr);
			resource->SetName(in_name);
		}
		/** Set the path method (for friends only) */
		template<typename RESOURCE_TYPE>
		static void SetResourcePath(RESOURCE_TYPE * resource, boost::filesystem::path const & in_path)
		{
			assert(resource != nullptr);
			resource->SetPath(in_path);
		}
	};

}; // namespace chaos
