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

		/** returns true whether the argument matches */
		bool MatchPath(boost::filesystem::path const & other_path) const;

	public:

		/** the path */
		boost::filesystem::path path;
		/** parent reference */
		FilePath const * parent_path = nullptr;		
	};


}; // namespace chaos
