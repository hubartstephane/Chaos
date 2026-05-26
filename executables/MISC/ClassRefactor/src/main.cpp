#include "chaos/Chaos.h"
#include <Windows.h>
#include <iostream>
#include <streambuf>

using namespace chaos;

//-----------------------------------------------------------

class DebugOutputBuf : public std::streambuf
{
protected:
	virtual int overflow(int c) override
	{
		if (c != EOF)
		{
			char ch = static_cast<char>(c);
			OutputDebugStringA(std::string(1, ch).c_str());
		}
		return c;
	}
};

DebugOutputBuf debugBuf;


//-----------------------------------------------------------

template<typename CLASS_TYPE>
class MyClass;

//-----------------------------------------------------------



//-----------------------------------------------------------


//-----------------------------------------------------------

#if 0



template<typename CLASS_TYPE>
struct MyParentClassOf
{
	using type = MyClassBase;
};

template<typename CLASS_TYPE>
	requires
(
	HasInternalSuperType<CLASS_TYPE> || HasExternalSuperType<CLASS_TYPE>
	)
	struct MyParentClassOf<CLASS_TYPE>
{
	using type = MyClass<SuperClass_t<CLASS_TYPE>>;
};

template<typename CLASS_TYPE>
using MyParentClassOf_t = typename MyParentClassOf<CLASS_TYPE>::type;


template<typename CLASS_TYPE>
class MyClass : public MyParentClassOf_t<CLASS_TYPE>
{
public:

	using Super = MyParentClassOf_t<CLASS_TYPE>;

	MyClass(std::string in_name, std::type_info const& in_info = typeid(CLASS_TYPE)) :
		Super(std::move(in_name), in_info) {
	}

	virtual CLASS_TYPE* CreateInstance() const
	{
		return new CLASS_TYPE;
	}


protected:


};

#else

class MyClassBase
{
public:

	MyClassBase(std::string in_name, std::type_info const& in_info) :
		name(std::move(in_name)),
		info(in_info) {}

	virtual ~MyClassBase() = default;

public:

	std::string name;
	std::type_info const& info;
};

template<typename CLASS_TYPE>
class MyClass : public MyClassBase
{
public:

	using Super = MyClassBase;

	MyClass(std::string in_name, std::type_info const& in_info = typeid(CLASS_TYPE)) :
		Super(std::move(in_name), in_info) {}

	virtual CLASS_TYPE* CreateInstance() const
	{
		return new CLASS_TYPE;
	}
};

template<typename CLASS_TYPE>
requires
(
	HasInternalSuperType<CLASS_TYPE> || HasExternalSuperType<CLASS_TYPE>
)
class MyClass<CLASS_TYPE> : public MyClass<SuperClass_t<CLASS_TYPE>>
{
public:

	using Super = MyClass<SuperClass_t<CLASS_TYPE>>;

	MyClass(std::string in_name, std::type_info const& in_info = typeid(CLASS_TYPE)) :
		Super(std::move(in_name), in_info){}

	virtual CLASS_TYPE* CreateInstance() const override
	{
		return new CLASS_TYPE;
	}
};




#endif






//-----------------------------------------------------------


class A
{
public:

	A() { std::cout << "A::A()" << std::endl; }

	virtual ~A() = default;

	static MyClass<A> const * GetStaticClass()
	{
		static MyClass<A> result("A");
		return &result;
	}

	virtual MyClass<A> const* GetClass() const
	{
		return A::GetStaticClass();
	}
};

class B : public A
{

public:

	using Super = A;

	B() { std::cout << "B::B()" << std::endl; }



	static MyClass<B> const* GetStaticClass()
	{
		static MyClass<B> result("B");
		return &result;
	}

	virtual MyClass<B> const* GetClass() const override
	{
		return B::GetStaticClass();
	}
};



int main(int argc, char ** argv, char ** env)
{
	std::cout.rdbuf(&debugBuf);



	bool internal_a = HasInternalSuperType<A>;
	bool internal_b = HasInternalSuperType<B>;

	bool external_a = HasExternalSuperType<A>;
	bool external_b = HasExternalSuperType<B>;

	std::cout << "---------------------------" << std::endl;
	std::cout << "internal_a: " << internal_a << std::endl;
	std::cout << "internal_b: " << internal_b << std::endl;
	std::cout << "---------------------------" << std::endl;
	std::cout << "external_a: " << external_a << std::endl;
	std::cout << "external_b: " << external_b << std::endl;
	std::cout << "---------------------------" << std::endl;


	MyClass<B> cb("B");

	cb.CreateInstance();

	//SuperClass_t<B> b;

	MyClass<A> const * cca = nullptr;
	MyClass<B> const* ccb = nullptr;


	return 0;
}