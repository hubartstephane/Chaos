#include <chaos/Manager.h>
#include <chaos/JSONTools.h>

namespace chaos
{

  bool Manager::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
  {
    return true;
  }

}; // namespace chaos