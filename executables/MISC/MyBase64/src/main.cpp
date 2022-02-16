#include <chaos/Chaos.h>

bool AreBuffersEquals(chaos::Buffer<char> const & b1, chaos::Buffer<char> const & b2)
{
  if (b1.bufsize != b2.bufsize)
    return false;

  size_t count = b1.bufsize;
  for (size_t i = 0; i < count; ++i)
    if (b1.data[i] != b2.data[i])
      return false;

  return true;
}

chaos::Buffer<char> GenerateRandomBuffer()
{
  size_t size = (size_t)(5000.0f + 5000.0f * chaos::MathTools::RandFloat());

  chaos::Buffer<char> result = chaos::SharedBufferPolicy<char>::NewBuffer(256 + size);
  if (result.data != nullptr)
  {
    for (size_t i = 0; i < 256; ++i)
      result.data[i] = (char)(i + 1); // ensure all numbers appear at least once
    for (size_t i = 256; i < 256 + size; ++i)
      result.data[i] = (char)(rand() % 32); // % 32 just to make sur some characters do not happen, and so is the compression better
  }
  return result;
}

chaos::Buffer<char> GetIpsumBuffer()
{
  static char * ipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

  int length = (int)strlen(ipsum) + 1;

  chaos::SparseWriteBuffer<> writer(512);

  for (int i = 0; i < 1; ++i) // multiple time the text, to be sure compression added data do not kill the benefit (for small data set)
    writer.Write(ipsum, length);

  chaos::Buffer<char> result = chaos::SharedBufferPolicy<char>::NewBuffer(writer.GetWrittenSize());
  writer.CopyToBuffer(result.data, result.bufsize);
  return result;

}

void TestCompression(chaos::Buffer<char> initial_buffer, char const * title)
{
  std::string compressed = chaos::MyBase64().Encode(initial_buffer);

  chaos::Buffer<char> uncompressed = chaos::MyBase64().Decode(compressed.c_str());

  bool success = AreBuffersEquals(initial_buffer, uncompressed);

  chaos::Log::Message("Compression %s : %d", title, success);
  if (success)
  {
    float compression_ratio = 100.0f * ((float)(compressed.length()) / (float)(initial_buffer.bufsize));

    chaos::Log::Message("  Source length      : %d", initial_buffer.bufsize);
    chaos::Log::Message("  Destination length : %d", compressed.length());
    chaos::Log::Message("  Compression ratio  : %f %%", compression_ratio);
  }
}

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::Buffer<char> f = chaos::FileTools::LoadFile("C:\\Users\\s.hubart\\Desktop\\totosave.txt");

  std::string str = f.data;

  str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
  str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());

  chaos::Buffer<char> uncompressed = chaos::MyBase64().Decode(str.c_str());
  std::ofstream file("C:\\Users\\s.hubart\\Desktop\\save.txt");
  file.write(uncompressed.data, uncompressed.bufsize);

return 0;




  return 0;



  chaos::MathTools::ResetRandSeed();

  TestCompression(GenerateRandomBuffer(), "Random text");

  TestCompression(GetIpsumBuffer(), "Ipsum text");

  chaos::WinTools::PressToContinue();

  return 0;
}


