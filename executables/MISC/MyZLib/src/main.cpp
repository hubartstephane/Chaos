#include <chaos/Chaos.h>

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
			result.data[i] = (char)(i + 1); // ensure all numbers appear at least once
		for (size_t i = 256 ; i < 256 + size ; ++i)
			result.data[i] = (char)(rand() % 32); // % 32 just to make sur some characters do not happen, and so is the compression better
	}
	return result;
}

chaos::Buffer<char> GetIpsumBuffer()
{
	static char * ipsum = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";

	int length = (int)strlen(ipsum) + 1;

	chaos::SparseWriteBuffer<> writer(512);

	for (int i = 0 ; i < 30 ; ++i) // multiple time the text, to be sure compression added data do not kill the benefit (for small data set)
		writer.Write(ipsum, length);

	chaos::Buffer<char> result = chaos::SharedBufferPolicy<char>::NewBuffer(writer.GetWrittenSize());
	writer.CopyToBuffer(result.data, result.bufsize);
	return result;

}

void TestCompression(chaos::Buffer<char> initial_buffer, char const * title)
{
	chaos::Buffer<char> compressed = chaos::MyZLib().Encode(initial_buffer);

	chaos::Buffer<char> uncompressed = chaos::MyZLib().Decode(compressed);

	bool success = AreBuffersEquals(initial_buffer, uncompressed);

	chaos::Log::Message("Compression %s : %d", title, success);
	if (success)
	{
		float compression_ratio = 100.0f * ((float)(compressed.bufsize) / (float)(initial_buffer.bufsize));

		chaos::Log::Message("  Source length      : %d", initial_buffer.bufsize);
		chaos::Log::Message("  Destination length : %d", compressed.bufsize);
		chaos::Log::Message("  Compression ratio  : %f %%", compression_ratio);
	}
}

void TestFromFile()
{
	chaos::Application * application = chaos::Application::GetInstance();
	if (application == nullptr)
		return;

	boost::filesystem::path const & resource_path = application->GetResourcesPath();

	chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(resource_path / "Ipsum.zip");
	if (buffer != nullptr)
	{
		chaos::Buffer<char> uncompressed = chaos::MyZLib().Decode(buffer);

		chaos::Log::Message("Uncompress Zip file      : %d", uncompressed.bufsize > 0);
	}
}

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		chaos::MathTools::ResetRandSeed();

		TestCompression(GenerateRandomBuffer(), "Random text");

		TestCompression(GetIpsumBuffer(), "Ipsum text");

		TestFromFile();

		chaos::WinTools::PressToContinue();

		return true;
	}
};

int main(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


