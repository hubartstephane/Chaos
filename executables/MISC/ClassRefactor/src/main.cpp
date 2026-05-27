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

//-----------------------------------------------------------

template<typename CLASS_TYPE>
class MyClass;

//-----------------------------------------------------------



//-----------------------------------------------------------

class MyClassBase
{
public:

	MyClassBase(std::string in_name, std::type_info const& in_info) :
		name(std::move(in_name)),
		info(in_info) {
	}

	virtual ~MyClassBase() = default;

public:

	std::string name;
	std::type_info const& info;
};

//-----------------------------------------------------------

#if 1



template<typename CLASS_TYPE>
struct MyClassParentClass
{
	using type = MyClassBase;
};

template<typename CLASS_TYPE>
requires
(
	HasInternalSuperType<CLASS_TYPE> || HasExternalSuperType<CLASS_TYPE>
)
struct MyClassParentClass<CLASS_TYPE>
{
	using type = MyClass<SuperClass_t<CLASS_TYPE>>;
};

template<typename CLASS_TYPE>
using MyClassParentClass_t = typename MyClassParentClass<CLASS_TYPE>::type;

//-----------------------------------------------------------


template<typename CLASS_TYPE>
class MyClass : public MyClassParentClass_t<CLASS_TYPE>
{
public:

	using Super = MyClassParentClass_t<CLASS_TYPE>;

	MyClass(std::string in_name, std::type_info const& in_info = typeid(CLASS_TYPE)) :
		Super(std::move(in_name), in_info) {}

	virtual CLASS_TYPE* CreateInstance() const
	{
		return new CLASS_TYPE;
	}



};

//-----------------------------------------------------------

#else



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

#define MY_CHAOS_DECLARE_OBJECT_CLASS(CLASS, PARENT_CLASS)\
public:\
using Super = PARENT_CLASS;\
static MyClass<CLASS> const * GetStaticClass(){ return CLASS##_class;}\
virtual MyClass<CLASS> const * GetClass() const { return CLASS##_class; }\
static inline MyClass<CLASS> const * CLASS##_class = MyDeclareObjectClass<CLASS>(#CLASS)

class A
{
protected:

	/** declare the class in the default C++ ClassManager (must be declared before the CHAOS_DECLARE_OBJECT_CLASS usage) */
	template<typename CLASS_TYPE>
	static MyClass<CLASS_TYPE> const* MyDeclareObjectClass(std::string name);

public:

	A() { std::cout << "A::A()" << std::endl; }

	virtual ~A() = default;

	static MyClass<A> const * GetStaticClass() { return A_class; }
	virtual MyClass<A> const* GetClass() const { return A_class; }
	static inline MyClass<A> const* A_class = MyDeclareObjectClass<A>("A");
};

template<typename CLASS_TYPE>
MyClass<CLASS_TYPE> const* A::MyDeclareObjectClass(std::string name)
{
	static MyClass<CLASS_TYPE> const result(std::move(name));
	return &result;
}

//-----------------------------------------------------------











class B : public A
{

public:

	MY_CHAOS_DECLARE_OBJECT_CLASS(B, A);

	B() { std::cout << "B::B()" << std::endl; }
};






int main(int argc, char ** argv, char ** env)
{
	DebugOutputBuf debugBuf;
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


//	B::GetStaticClass()->CreateInstance();


	//MyClass<B> cb("B");

	//cb.CreateInstance();

	//SuperClass_t<B> b;

	//B bbb;

	//MyClass<A> const * cca = nullptr;
	//MyClass<B> const* ccb = nullptr;


	return 0;
}