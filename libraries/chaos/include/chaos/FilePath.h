#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	class FilePath
	{
	public:

		/** constructor */
		FilePath(char const * in_filename, FilePath const * in_parent_path = nullptr);
		/** constructor */
		FilePath(std::string const & in_filename, FilePath const * in_parent_path = nullptr);
		/** constructor */
		FilePath(boost::filesystem::path const & in_filename, FilePath const * in_parent_path = nullptr);

	public:

		/** the path */
		boost::filesystem::path path;
		/** parent reference */
		FilePath const * parent_path = nullptr;		
	};


}; // namespace chaos
