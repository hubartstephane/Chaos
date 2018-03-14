#include <chaos/FilePath.h>

namespace chaos
{

	// ==============================================================
	// FILE PATH
	// ==============================================================

	FilePath::FilePath()
	{

	}

	FilePath::FilePath(char const * in_basic_path, boost::filesystem::path const * in_reference_path):
		basic_path(in_basic_path),
		reference_path(in_reference_path)
	{

	}

	FilePath::FilePath(std::string const & in_string_path, boost::filesystem::path const * in_reference_path):
		string_path(&in_string_path),
		reference_path(in_reference_path)
	{

	}

	FilePath::FilePath(boost::filesystem::path const & in_filesystem_path, boost::filesystem::path const * in_reference_path):
		filesystem_path(&in_filesystem_path),
		reference_path(in_reference_path)
	{
	}

	boost::filesystem::path const & FilePath::GetResolvedPath() const
	{
		if (resolved_path.empty()) // compute the path or use cache result
		{
			boost::filesystem::path temp_path; // for conversion to boost object if necessary

			boost::filesystem::path const * p = nullptr; // a pointer on a boost path

			// ensure we have a boost path object on which we can work
			if (basic_path != nullptr)
			{
				temp_path = basic_path;
				p         = &temp_path;
			}
			else if (string_path != nullptr)
			{
				temp_path = string_path->c_str();
				p         = &temp_path;
			}
			else if (filesystem_path != nullptr)
			{
				p = filesystem_path;		
			}
			else
				return resolved_path; // error 

			// cannot resolve or it is not necessary
			if (reference_path == nullptr || reference_path->empty() || p->is_absolute())
			{
				if (p == &temp_path)
					resolved_path = std::move(temp_path); // can steal the temp_path
				else
					resolved_path = temp_path; // must copy the temp_path		
			}
			// resolve the path
			else
			{
				resolved_path = (reference_path->parent_path() / temp_path).normalize(); // resolve
			}
		}
		return resolved_path;
	}






}; // namespace chaos
