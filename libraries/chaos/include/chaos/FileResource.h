#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	/** an object that have a name (the difference with NamedObject is the tag) */
	class NamedResource
	{
		friend class ResourceFriend;

	public:

		/** get the name of the object */
		char const * GetName() const { return name.c_str(); }

	protected:

		/** Set the name method (for friends only) */
		void SetName(char const * in_name);

	protected:

		/** the name of the object */
		std::string name;
	};

	/** an object that have a path and a timestamp */
	class FileResource
	{
		friend class ResourceFriend;

	public:

		/** get the path of the object */
		boost::filesystem::path const & GetPath() const { return path; }
		/** get the file last write time */
		std::time_t GetFileTimestamp() const { return file_timestamp; }

	protected:

		/** Set the path method (for friends only) */
		void SetPath(boost::filesystem::path const & in_path);

	protected:

		/** the path of the object */
		boost::filesystem::path path;
		/** the file timestamp */
		std::time_t file_timestamp = 0;
	};

	/** an utility class just to access protected methods */
	class ResourceFriend
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
