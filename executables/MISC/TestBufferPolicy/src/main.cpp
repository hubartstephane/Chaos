#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>
#include <chaos/AllocatorTools.h>
#include <chaos/Buffer.h>

// ---------------------------------------------------------------------

class A 
{
public:

	A(){ std::cout << "new A    : " << this << std::endl; }
	~A(){ std::cout << "delete A : " << this << std::endl; }

	int S[17];
};

class B 
{
public:

	B(){ std::cout << "new B    : " << this << std::endl; }
	~B(){ std::cout << "delete B : " << this << std::endl; }

	int S[5];
};

// ---------------------------------------------------------------------



// ---------------------------------------------------------------------





class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{

		std::cout << "-- CREATE SHARED BUFFER 1 --" << std::endl;

		auto a = chaos::SharedNonOptimzedBufferPolicy<A>::NewBuffer(10);

		std::cout << "-- CREATE SHARED BUFFER 2--" << std::endl;

		auto b = chaos::SharedBufferPolicy<A>::NewBuffer(5);

		std::cout << "-- COPY BUFFER (BUFFER 1 DESTROYED) => BUFFER 2 REFERENCE = 2 --" << std::endl;

		a = b;

		std::cout << "-- 1 REFERENCE BUFFER 2 REMOVED : NO DESTRUCTION --" << std::endl;

		b = chaos::Buffer<A>();

		std::cout << "-- 1 REFERENCE BUFFER 2 REMOVED : HERE IS DESTRUCTION --" << std::endl;

		a = chaos::Buffer<A>();

		chaos::WinTools::PressToContinue();

		return true;
	}
};


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


