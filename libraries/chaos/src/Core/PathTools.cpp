#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace PathTools
	{
		std::string PathToName(boost::filesystem::path const & path)
		{
			return path.filename().stem().string();
		}

		boost::filesystem::path FindAbsolutePath(boost::filesystem::path const & reference_path, boost::filesystem::path const & path)
		{
			if (path.is_absolute())
				return path.lexically_normal().make_preferred();

			boost::filesystem::path result;
			FileTools::WithFile(reference_path, [&result, &path](boost::filesystem::path const & p)
			{
				boost::filesystem::file_status status = boost::filesystem::status(p);

				if (status.type() == boost::filesystem::file_type::directory_file)
				{
					result = (p / path).lexically_normal().make_preferred();
					return true;
				}
				else if (status.type() == boost::filesystem::file_type::regular_file)
				{
					boost::filesystem::path parent_path = p.parent_path();
					result = (parent_path / path).lexically_normal().make_preferred();
					return true;
				}
				return false; // don't stop
			});
            return result;
		}

	}; // namespace PathTools

}; // namespace chaos
