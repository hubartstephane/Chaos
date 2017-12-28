#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{

  class FileManager : public ReferencedObject
  {
  public:

    /** find corresponding full path */
    boost::filesystem::path ResolvePath(char const * path) const;
    /** find corresponding full path */
    boost::filesystem::path ResolvePath(boost::filesystem::path const & path) const;

    /** add a directory in the known directories */
    void AddDirectory(boost::filesystem::path const & path, bool recursive);

  protected:

    /** the known directories */
    std::vector<boost::filesystem::path> directories;
  };

}; // namespace chaos

