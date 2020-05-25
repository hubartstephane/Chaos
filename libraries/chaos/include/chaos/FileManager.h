#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
#include <chaos/Buffer.h>

namespace chaos
{
	class LoadFileResult
	{
	public:

		/** the loading file result */
		Buffer<char> buffer;
		/** the path due to path resolution */
		boost::filesystem::path path;

		/** implicit conversion into buffer */
		operator Buffer<char> ();
	};

	class FileManager : public Object
	{
	public:

		/** find corresponding full path */
		boost::filesystem::path ResolvePath(char const * path, boost::filesystem::path const & context_path = boost::filesystem::path()) const;
		/** find corresponding full path */
		boost::filesystem::path ResolvePath(boost::filesystem::path const & path, boost::filesystem::path const & context_path = boost::filesystem::path()) const;

		/** load a file (with path resolution) */
		LoadFileResult LoadFile(char const * path, bool ascii, boost::filesystem::path const & context_path = boost::filesystem::path()) const;
		/** load a file (with path resolution) */
		LoadFileResult LoadFile(boost::filesystem::path const & path, bool ascii, boost::filesystem::path const & context_path = boost::filesystem::path()) const;

		/** add a directory in the known directories */
		void AddDirectory(boost::filesystem::path const & path, bool recursive);

	protected:

		/** the known directories */
		std::vector<boost::filesystem::path> directories;
	};

}; // namespace chaos

