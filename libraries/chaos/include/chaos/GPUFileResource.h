#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{
  class GPUFileResource : public GPUResource
  {
  public:

    /** constructor */
    GPUFileResource(char const * in_name = nullptr)
    {
      if (in_name != nullptr)
        name = in_name;
    }

    /** get the name of the object */
    char const * GetName() const { return name.c_str(); }
    /** get the path of the object */
    boost::filesystem::path const & GetPath() const { return path; }

  protected:

    /** the name of the object */
    std::string name;
    /** the path of the object */
    boost::filesystem::path path;
  };


}; // namespace chaos
