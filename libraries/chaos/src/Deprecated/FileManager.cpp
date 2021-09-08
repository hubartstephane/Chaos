#include <chaos/Chaos.h>

namespace chaos
{

	LoadFileResult::operator Buffer<char> ()
	{
		return buffer;
	}

	boost::filesystem::path FileManager::ResolvePath(char const * path, boost::filesystem::path const & context_path) const
	{
		assert(path != nullptr);
		return ResolvePath(boost::filesystem::path(path), context_path);
	}

	boost::filesystem::path FileManager::ResolvePath(boost::filesystem::path const & path, boost::filesystem::path const & context_path) const
	{
		// nothing to do if the path is complete
		if (path.is_absolute())
			return path;

		// try to ass





		return boost::filesystem::path();
	}

	LoadFileResult FileManager::LoadFile(char const * path, bool ascii, boost::filesystem::path const & context_path) const
	{
		return LoadFile(boost::filesystem::path(path), ascii, context_path);
	}

	LoadFileResult FileManager::LoadFile(boost::filesystem::path const & path, bool ascii, boost::filesystem::path const & context_path) const
	{
		LoadFileResult result;
		result.path = ResolvePath(path, context_path);
		if (!result.path.empty())
			result.buffer = FileTools::LoadFile(result.path, ascii);
		return result;
	}

	void FileManager::AddDirectory(boost::filesystem::path const & path, bool recursive)
	{
		// test whether the path correspond to a directory
		if (!boost::filesystem::is_directory(path))
			return;

		// test whether the element is already in the list
		boost::filesystem::path p = path.lexically_normal().make_preferred();
		if (std::find(directories.begin(), directories.end(), p) != directories.end())
			return;

		// add the element to the directory
		directories.push_back(p);

		// recursive action
		if (recursive)
		{
			FileTools::ForEachRedirectedDirectoryContent(p, [this, recursive](boost::filesystem::path const& p)
			{
				AddDirectory(p, recursive);
				return false; // don't stop
			});
		}
	}

}; // namespace chaos
