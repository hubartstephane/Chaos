#include <chaos/FilePath.h>

namespace chaos
{

  // ==============================================================
  // FILE PATH
  // ==============================================================

  FilePathParam::FilePathParam(char const * basic_path)
  {
    ResolvePath(basic_path, nullptr, nullptr, nullptr);
  }

  FilePathParam::FilePathParam(std::string const & string_path)
  {
    ResolvePath(nullptr, &string_path, nullptr, nullptr);
  }

  FilePathParam::FilePathParam(boost::filesystem::path const & filesystem_path)
  {
    ResolvePath(nullptr, nullptr, &filesystem_path, nullptr);
  }

  FilePathParam::FilePathParam(char const * basic_path, boost::filesystem::path const & reference_path)
  {
    ResolvePath(basic_path, nullptr, nullptr, &reference_path);
  }

  FilePathParam::FilePathParam(std::string const & string_path, boost::filesystem::path const & reference_path) 
  {
    ResolvePath(nullptr, &string_path, nullptr, &reference_path);
  }

  FilePathParam::FilePathParam(boost::filesystem::path const & filesystem_path, boost::filesystem::path const & reference_path)
  {
    ResolvePath(nullptr, nullptr, &filesystem_path, &reference_path);
  }

  void FilePathParam::ResolvePath(char const * basic_path, std::string const * string_path, boost::filesystem::path const * filesystem_path, boost::filesystem::path const * reference_path)
  {
    boost::filesystem::path temp_path; // for conversion to boost object if necessary

    boost::filesystem::path const * p = nullptr; // a pointer on a boost path
                                                 
    // ensure we have a boost path object on which we can work
    if (basic_path != nullptr)
    {
      temp_path = basic_path;
      p = &temp_path;
    }
    else if (string_path != nullptr)
    {
      temp_path = string_path->c_str();
      p = &temp_path;
    }
    else if (filesystem_path != nullptr)
    {
      p = filesystem_path;
    }
    else
      return;
                            
    // cannot resolve or it is not necessary
    if (reference_path == nullptr || reference_path->empty() || p->is_absolute())
    {
      if (p == &temp_path)
        resolved_path = std::move(temp_path); // can steal the temp_path
      else
        resolved_path = *p; // must copy the temp_path		
    }
    // resolve the path
    else
    {
      resolved_path = (reference_path->parent_path() / *p).normalize(); // resolve
    }
  }

  boost::filesystem::path const & FilePathParam::GetResolvedPath() const
  {
    return resolved_path;
  }

}; // namespace chaos
