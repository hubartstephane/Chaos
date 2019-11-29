
#include <chaos/BoostTools.h>

namespace chaos
{
	namespace BoostTools
	{
		std::string PathToName(boost::filesystem::path const & path)
		{
			return path.filename().stem().string();	  
		}

		boost::filesystem::path FindAbsolutePath(boost::filesystem::path const & reference_path, boost::filesystem::path const & path)
		{
			if (path.is_absolute())
				return path.lexically_normal().make_preferred();

			if (boost::filesystem::is_directory(reference_path))
			{
				return (reference_path / path).lexically_normal().make_preferred();
			}
			else if (boost::filesystem::is_regular_file(reference_path))
			{
				boost::filesystem::path p = reference_path.parent_path();
				return (p / path).lexically_normal().make_preferred();
			}
            return boost::filesystem::path();
		}

	}; // namespace BoostTools

}; // namespace chaos
