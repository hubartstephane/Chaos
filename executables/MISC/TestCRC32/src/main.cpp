#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  char const * source1 = "abcdef";
  char const * source2 = "ghijkl";

  boost::crc_32_type crc;

  // compute checksum
  crc.process_bytes(source1, 6);
  unsigned int v1 = crc.checksum();

  std::cout << "checksum 1 : " << v1 << std::endl;

  // compute another time the checksum : still equal
  crc.reset();
  crc.process_bytes(source1, 6);
  unsigned int v2 = crc.checksum();

  std::cout << "checksum 2 : " << v2 << std::endl;

  // compute another checksum on another value (source1 + source2) : ensure different
  crc.reset();
  crc.process_bytes(source1, 6);
  crc.process_bytes(source2, 6);
  unsigned int v3 = crc.checksum();

  std::cout << "checksum 3 : " << v3 << std::endl;


  chaos::WinTools::PressToContinue();

  return 0;
}


