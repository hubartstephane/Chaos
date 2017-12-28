#include <chaos/FileManager.h>

namespace chaos
{

  boost::filesystem::path FileManager::ResolvePath(char const * path) const
  {
    assert(path != nullptr);
    return ResolvePath(boost::filesystem::path(path));
  }

  boost::filesystem::path FileManager::ResolvePath(boost::filesystem::path const & path) const
  {
    // nothing to do if the path is complete
    if (path.is_absolute())
      return path;

    // try to ass





    return boost::filesystem::path();
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
      boost::filesystem::directory_iterator end;
      for (boost::filesystem::directory_iterator it(p); it != end; ++it)
        AddDirectory(*it, recursive);
    }
  }

}; // namespace chaos
