
#include <chaos/BoostTools.h>

namespace chaos
{
  namespace BoostTools
  {
    boost::filesystem::path FindAbsolutePath(boost::filesystem::path const & reference_path, boost::filesystem::path const & path)
    {
      if (path.is_absolute())
        return path.lexically_normal().make_preferred();

      boost::filesystem::path p = reference_path.parent_path();
      return (p / path).lexically_normal().make_preferred();
    }

  }; // namespace BoostTools

}; // namespace chaos
