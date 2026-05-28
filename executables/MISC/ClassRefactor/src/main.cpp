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

class MyClassManager;

//-----------------------------------------------------------



//-----------------------------------------------------------

class MyClassBase
{
public:

	MyClassBase(std::string in_name, std::type_info const& in_info):
		name(std::move(in_name)),
		info(in_info){}

	/** destructor */
	virtual ~MyClassBase() = default;

	/** gets the class name */
	std::string const& GetClassName() const { return name; }
	/** gets the short name */
	std::string const& GetShortName() const { return short_name; }
	/** gets the class size */
	size_t GetClassSize() const { return class_size; }
	/** returns whether the class has been registered */
	bool IsDeclared() const { return declared; }
	/** gets the parent class */
	MyClassBase const* GetParentClass() const { return parent; }
	/** gets the class manager */
	MyClassManager* GetClassManager() const { return manager; } // no need to have a manager const

protected:
public:

	/** the optional name of the class */
	std::string name;
	/** the optional short name of the class */
	std::string short_name;
	/** get class size */
	size_t class_size = 0;
	/** whether the class has been fully declared */
	bool declared = false;
	/** the type_info for the class */
	std::type_info const & info;
	/** the parent of the class */
	MyClassBase const* parent = nullptr;
	/** the manager for this class */
	MyClassManager* manager = nullptr;
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

class MyClassFindResult
{
public:

	template<typename CLASS_TYPE>
	operator CLASS_TYPE * () const
	{
		return nullptr;
	};

protected:


};

//-----------------------------------------------------------

class MyClassManager
{
public:

	static MyClassManager & GetCPPManagerInstance()
	{
		static MyClassManager result;
		return result;
	}

	template<typename CLASS_TYPE>
	MyClass<CLASS_TYPE> const * RegisterCPPClass(std::string name)
	{
		std::type_info const & info = typeid(CLASS_TYPE);

		for (MyClassBase const* cls : classes)
			if (cls->info == info)
				return auto_cast_checked(cls);
	
		MyClass<CLASS_TYPE> * result = new MyClass<CLASS_TYPE>(std::move(name));
		if (result == nullptr)
			return nullptr;
		classes.push_back(result);
		return result;
	}

	MyClassFindResult FindClass(char const* name, FindClassFlags flags)
	{
		MyClassFindResult result;
	
	
		return result;
	}

protected:

	std::vector<MyClassBase const *> classes;
};




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
	return MyClassManager::GetCPPManagerInstance().RegisterCPPClass<CLASS_TYPE>(std::move(name));
}

//-----------------------------------------------------------











class B : public A
{

public:

	MY_CHAOS_DECLARE_OBJECT_CLASS(B, A);

	B() { std::cout << "B::B()" << std::endl; }
};




class U1 {};
class U2 : public U1 
{
public:

	using Super = U1;
};


class U3 : public U2 {};
CHAOS_DECLARE_EXTERNAL_SUPER(U3, U2);


class U4 : public U3 
{
public:

	using Super = U3;
};
CHAOS_DECLARE_EXTERNAL_SUPER(U4, EmptyClass);



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

	bool a1 = HasSuperType<U1>;
	bool a2 = HasSuperType<U2>;
	bool a3 = HasSuperType<U3>;
	bool a4 = HasSuperType<U4>;
	std::cout << "---------------------------" << std::endl;
	std::cout << "HasSuperType U1: " << a1 << std::endl;
	std::cout << "HasSuperType U2: " << a2 << std::endl;
	std::cout << "HasSuperType U3: " << a3 << std::endl;
	std::cout << "HasSuperType U4: " << a4 << std::endl;
	std::cout << "---------------------------" << std::endl;

	auto ppp = sizeof(EmptyClass);

	MyClassFindResult fr;

	MyClass<A> * ca = fr;


//	B::GetStaticClass()->CreateInstance();


	//MyClass<B> cb("B");

	//cb.CreateInstance();

	//SuperClass_t<B> b;

	//B bbb;

	//MyClass<A> const * cca = nullptr;
	//MyClass<B> const* ccb = nullptr;


	return 0;
}