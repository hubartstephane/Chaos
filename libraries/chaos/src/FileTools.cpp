#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/StringTools.h>
#include <chaos/AllocatorTools.h>

namespace chaos
{

char const * FileTools::GetFilenameExtension(char const * filename)
{
  assert(filename != NULL);
  char const * result = strchr(filename, '.');
  while (result != NULL)
  {
    char const * next_result = strchr(result + 1, '.');
    if (next_result == NULL)
      break;
    result = next_result;
  }
  if (result != NULL)
  {
    if (result[0] == '.') // do not include separator
      ++result;
    if (result[0] == 0) // empty extension is considered has no extension
      result = NULL;
  }
  return result;
}

bool FileTools::IsTypedFile(char const * filename, char const * expected_ext)
{
  assert(filename     != NULL);
  assert(expected_ext != NULL);
  char const * ext = GetFilenameExtension(filename);
  if (ext != NULL)
    return (_stricmp(ext, expected_ext) == 0);
  return false;
}

Buffer<char> FileTools::LoadFile(boost::filesystem::path const & filename, bool ascii)
{
  return LoadFile(filename.string().c_str(), ascii);
}

Buffer<char> FileTools::LoadFile(char const * filename, bool ascii)
{
  assert(filename != NULL);

  Buffer<char> result;

#if _WIN32

  HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    DWORD FileSize = GetFileSize(hFile, NULL);
    if (FileSize != INVALID_FILE_SIZE)
    {
      result = SharedBufferPolicy<char>::NewBuffer(FileSize + (ascii? 1 : 0));

      if (result != nullptr)
      {
        DWORD FileRead = 0;
        if (!ReadFile(hFile, result.data, FileSize, &FileRead, NULL))
          result = Buffer<char>();
        else if (ascii)
          result.data[FileSize] = 0; 
      }
    }
    CloseHandle(hFile);
  }
  //else
  //  LogTools::Log("FileTools::LoadFile : failed to load file [%s]", filename);

#elif _LINUX

  FILE * file = fopen(filename, "r");
  if (file != NULL)
  {
    struct stat st;
    if (fstat(fileno(file), &st) != -1)
    {
      if (result.Allocate(st.st_size + (ascii? 1 : 0)) != NULL)
      {
        if (fread(result.data, st.st_size, 1, file) != 1)
          result.Release(true);
        else if (ascii)
          result.buffer[st.st_size] = 0;
      }
    }

    fclose(file);
  }
#endif // _LINUX

  return result;
}
bool FileTools::CreateTemporaryDirectory(char const * pattern, boost::filesystem::path & result)
{
  boost::filesystem::path temp_path = boost::filesystem::temp_directory_path();
  
  boost::filesystem::path uniq_path = (pattern == nullptr)?
    boost::filesystem::unique_path(temp_path / "%%%%-%%%%-%%%%-%%%%") :
    boost::filesystem::unique_path(temp_path / StringTools::Printf("%s_%%%%-%%%%-%%%%-%%%%", pattern));
    
  if (boost::filesystem::create_directories(uniq_path))
  {
    result = std::move(uniq_path);
    return true;
  }

  return false;
}

}; // namespace chaos