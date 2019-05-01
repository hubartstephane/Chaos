#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 
#include "Ludum43Game.h"

#include <chaos/EmptyClass.h>
#include <chaos/MetaProgramming.h>


template<typename T> 
class Casted
{
protected:

	Casted(T * in_ptr) : ptr(in_ptr) {}

public:

	template<typename U>
	operator U * () const
	{
		return dynamic_cast<U *>(ptr);

	}

	template<typename T>
	friend Casted<T> cast(T * ptr) { return Casted<T>(ptr); }

protected:

	T * ptr = nullptr;

};

template<typename T>
class ConstCasted
{
protected:

	ConstCasted(T * in_ptr) : ptr(in_ptr) {}

public:
	
	template<typename U>
	operator U const * () const
	{
		return dynamic_cast<const U *>(ptr);
	}

	template<typename T>
	friend ConstCasted<T> cast(T const * ptr) { return ConstCasted<T>(ptr); }

protected:

	T const * ptr = nullptr;

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






int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
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
	}
	{
		A const * a = new A;
		B const * b = new B;
		C const * c = nullptr; // new C;

		A const * a1 = cast(a);
		B const * b1 = cast(a);
		C const * c1 = cast(a);

		A const * a2 = cast(b);
		B const * b2 = cast(b);
		C const * c2 = cast(b);

		A const * a3 = cast(c);
		B const * b3 = cast(c);
		C const * c3 = cast(c);
	}


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


