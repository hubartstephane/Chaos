#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/MathTools.h>
#include <chaos/MyZLib.h>

bool AreBuffersEquals(chaos::Buffer<char> const & b1, chaos::Buffer<char> const & b2)
{
	if (b1.bufsize != b2.bufsize)
		return false;

	size_t count = b1.bufsize;
	for (size_t i = 0 ; i < count ; ++i)
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
		for (size_t i = 0 ; i < 256 ; ++i)
			result.data[i] = (char)(i); // ensure all numbers appear at least once
		for (size_t i = 256 ; i < 256 + size ; ++i)
			result.data[i] = (char)(rand() % 32); // % 32 just to make sur some characters do not happen, and so is the compression better				
	}
	return result;
}

int _tmain(int argc, char ** argv, char ** env)
{
	chaos::WinTools::AllocConsoleAndRedirectStdOutput();
	
	chaos::MathTools::ResetRandSeed();

	chaos::Buffer<char> initial_buffer = GenerateRandomBuffer();

	chaos::Buffer<char> compressed = chaos::MyZLib().Encode(initial_buffer);

	chaos::Buffer<char> uncompressed = chaos::MyZLib().Decode(compressed);

	bool success = AreBuffersEquals(initial_buffer, uncompressed);


	chaos::WinTools::PressToContinue();

	return 0;
}


