#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 
#include "Ludum43Game.h"

#include <chaos/EmptyClass.h>
#include <chaos/MetaProgramming.h>


template<typename T> 
class Casted
{
public:

	Casted(T * in_ptr) : ptr(in_ptr) 
	{
		//assert(in_ptr != nullptr;)
	}

	template<typename U>
	operator U * ()
	{
		return dynamic_cast<U *>(ptr);

	}

protected:

	T * ptr = nullptr;

};

class A
{
public:

	virtual ~A() {}

};

class B
{
public:

	virtual ~B() {}

};

class C : public A
{
public:

	virtual ~C() {}

};

template<typename T>
Casted<T> cast(T * ptr) { return Casted<T>(ptr); }


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	A * a = new A;
	B * b = new B;
	C * c = nullptr; // new C;

	A * a1 = cast(a);
	B * b1 = cast(a);
	C * c1 = cast(a);

	A * a2 = cast(b);
	B * b2 = cast(b);
	C * c2 = cast(b);

	A * a3 = cast(c);
	B * b3 = cast(c);
	C * c3 = cast(c);


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


