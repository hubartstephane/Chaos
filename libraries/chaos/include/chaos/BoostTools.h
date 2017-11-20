#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
  namespace BoostTools
  {
    /** returns (if possible) the absolute path. Use the reference_path to know about the root directory */
    boost::filesystem::path FindAbsolutePath(boost::filesystem::path const & reference_path, boost::filesystem::path const & path);

  }; // namespace BoostTools

}; // namespace chaos
