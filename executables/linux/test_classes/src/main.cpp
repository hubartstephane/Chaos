#include <stdlib.h>
#include <stdio.h>
#include <iostream>

class Object;

class Class
{
public:
	
	virtual Object* CreateObject() const;
};

class Object
{
public:

	using Class = ::Class;
	#if 0
	class Class
	{
	public:
		
		virtual Object* CreateObject() const { return new Object; }		
	};
	#endif
	
	virtual Class const * GetClass() const
	{
		return GetStaticClass();
	}
	
	static Class const * GetStaticClass()
	{
		static Class result;
		return &result;
	}

	Object()
	{
		std::cout << "Object constructor" << std::endl;
	}
	
};

Object* Class::CreateObject() const { return new Object; }	

class Object2 : public Object
{
public:

	class Class : public Object::Class
	{
	public:
		
		virtual Object2* CreateObject() const { return new Object2; }		
	};

	virtual Class const * GetClass() const
	{
		return GetStaticClass();
	}
	
	static Class const * GetStaticClass()
	{
		static Class result;
		return &result;
	}

	Object2()
	{
		std::cout << "Object2 constructor" << std::endl;
	}
};

//////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
	Class const * c1  = Object2::GetStaticClass();
	c1->CreateObject();

	Object::Class const * cls = Object::GetStaticClass();
	cls->CreateObject();
	return 0;
}
 