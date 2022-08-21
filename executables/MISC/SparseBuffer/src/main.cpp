
#include "chaos/Chaos.h"

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		chaos::SparseWriteBuffer<> write_buf(15);

		for (int i = 0; i < 100 ; ++i)
			write_buf << (int)('a' + (i % 26));


		write_buf << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

		write_buf.Clear(false);
		for (int i = 0 ; i < 1000; ++i)
			write_buf << (int)0x44;




		//write_buf << "abcd";
		//write_buf << "xklcjvbkjxcnvbkjnxcvjkbnxlckvjnbkjxncv";


		size_t s = write_buf.GetWrittenSize();

		char * buffer = new char[s];
		if (buffer != nullptr)
			write_buf.CopyToBuffer(buffer, s);

		int * b = (int*)buffer;
		for (int i = 0 ; i < 1000 ;  ++i)
			if (b[i] != 0x44)
				b = b;

		chaos::WinTools::PressToContinue();

		return true;
	}
};

int main(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


