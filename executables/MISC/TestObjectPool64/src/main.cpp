#include "chaos/Chaos.h"

using namespace chaos;

class DefaultConstructible
{
public:



};

class NoDefaultConstructible
{
public:

	NoDefaultConstructible(int i) {}
};

class MyApplication : public chaos::Application
{
	CHAOS_DECLARE_OBJECT_CLASS(MyApplication, chaos::Application);

protected:

	void Test1()
	{
		ObjectPool64<DefaultConstructible> pool;
		for (uint64_t i = 0 ; i < 65 ; ++i)
		{
			DefaultConstructible * elem = pool.Allocate();
			if (elem == nullptr)
			{
				std::cout << "Test1: fail to allocate elem: " << i << std::endl;
			}
		}	
	}

	void Test2()
	{
		ObjectPool64<NoDefaultConstructible> pool;
		for (uint64_t i = 0; i < 65; ++i)
		{
			NoDefaultConstructible* elem = pool.Allocate(i);
			if (elem == nullptr)
			{
				std::cout << "Test2: fail to allocate elem: " << i << std::endl;
			}
		}
	}

	virtual int Main() override
	{
		Test1();
		Test2();
		return 0;
	}
};

int main(int argc, char ** argv, char ** env)
{
	StreamToDebugOutputBuf debugBuf;
	std::cout.rdbuf(&debugBuf);

	chaos::ApplicationData application_data;
	return chaos::RunApplication<MyApplication>(argc, argv, env, &application_data);
}