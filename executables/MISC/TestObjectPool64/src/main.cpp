#include "chaos/Chaos.h"

using namespace chaos;

class DefaultConstructible
{
public:



};

class NoDefaultConstructible
{
public:

	NoDefaultConstructible(uint64_t i) {}
};

class Recyclable
{
public:

	void Initialize()
	{
		std::cout << "Test3: Initialize called" << std::endl;
	}

	void Recycle()
	{
		std::cout << "Test3: Recycle called" << std::endl;
	}
};

class BadInitializeParams
{
public:

	BadInitializeParams()
	{
	}

	BadInitializeParams(bool)
	{
		std::cout << "Test4: BadInitializeParams constructor called" << std::endl;
	}

	~BadInitializeParams()
	{
		std::cout << "Test4: ~BadInitializeParams called" << std::endl;
	}


	void Initialize()
	{
		std::cout << "Test4: Initialize called" << std::endl;
	}

	void Recycle()
	{
		std::cout << "Test4: Recycle called" << std::endl;
	}
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

	void Test3()
	{
		ObjectPool64<Recyclable> pool;
		pool.Free(pool.Allocate());
		pool.Allocate();
	}

	void Test4()
	{
		ObjectPool64<BadInitializeParams> pool;

		BadInitializeParams * last_created = nullptr;
		for (int i = 0 ; i < 64 ; ++i)
			last_created = pool.Allocate();
		pool.Free(last_created); // should be recycled

		pool.Allocate(true); // bad parameters. require destruction + constructor
	}

	virtual int Main() override
	{
		Test1();
		Test2();
		Test3();
		Test4();
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