#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/Class.h>
#include <chaos/Object.h>
#include <chaos/LogTools.h>

class A : public chaos::Object 
{
	CHAOS_OBJECT_DECLARE_CLASS2(A, chaos::Object);

};

class B : public A 
{
	CHAOS_OBJECT_DECLARE_CLASS2(B, A);

};

class C : public B 
{
	CHAOS_OBJECT_DECLARE_CLASS2(C, B);

};

class D 
{

};


template<typename T> 
class SubClassOf
{
public:

	/** constructor */
	SubClassOf() : SubClassOf(T::GetStaticClass())
	{
	}
	/** constructor */
	SubClassOf(SubClassOf const& src) = default;
	/** constructor */
	SubClassOf(chaos::Class const * src)
	{
		if (src != nullptr)
		{



			chaos::Class const* base_class = chaos::Class::FindClass<T>();


			auto ppp = src->InheritsFrom(base_class, true);

			if (base_class == nullptr)
			{
				chaos::LogTools::Error("SubClassOf constructor : FindClass<T> failure");
			}
			else if (src->InheritsFrom(base_class, true) != chaos::InheritanceType::YES)
			{
				chaos::LogTools::Error("SubClassOf constructor : src class does not inherit from base_class");
			}
			else
			{
				internal_class = src;
			}
		}
	}
	/** constructor */
	template<typename U>
	SubClassOf(SubClassOf<U> const& src) : SubClassOf(src.GetInternalClass())
	{
		int u = 0;
		++u;
	}

	/** validity operator */
	operator bool() const { return IsValid(); }
	/** returns whether the object is valid */
	bool IsValid() const { return (internal_class != nullptr);  }

	/** method to create an instance of the object */
	T* CreateInstance() const
	{
		if (internal_class == nullptr)
			return nullptr;
		return (T*)internal_class->CreateInstance();
	}

	/** assign operator */
	SubClassOf<T>& operator = (chaos::Class const* src)
	{
		*this = SubClassOf<T>(src);
		return *this;
	}
	/** assign operator */
	template<typename U>
	SubClassOf<T>& operator = (SubClassOf<U> const & src)
	{
		*this = SubClassOf<T>(src);
		return *this;
	}

	/** get the internal class */
	chaos::Class const* GetInternalClass() const { return internal_class; }

protected:

	/** the internal class used for that */
	chaos::Class const * internal_class = nullptr;
};



class T {};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();


  SubClassOf<chaos::Object> a1 = A::GetStaticClass();
  SubClassOf<A> a2 = A::GetStaticClass();
  SubClassOf<B> a3 = A::GetStaticClass();
  SubClassOf<A> a4 = B::GetStaticClass();
  SubClassOf<B> a5 = B::GetStaticClass();


  SubClassOf<A> a6 = SubClassOf<B>();
  SubClassOf<B> a7 = SubClassOf<B>();
  SubClassOf<A> a8 = SubClassOf<A>();
  SubClassOf<B> a9 = SubClassOf<A>();


  bool b1 = a1;
  bool b2 = a2;
  bool b3 = a3;
  bool b4 = a4;
  bool b5 = a5;

  bool b6 = a6;
  bool b7 = a7;
  bool b8 = a8;
  bool b9 = a9;

  auto pp = chaos::Class::FindClass<T>();







//  SubClassOf<chaos::Object> a = chaos::Object::GetStaticClass();
  SubClassOf<chaos::Object> b = A::GetStaticClass();


  SubClassOf<A> xxx = SubClassOf<A>();

  //a = b;

#if 0

	chaos::Class::DeclareClass<A>();
	chaos::Class::DeclareClass<B, A>();
	chaos::Class::DeclareClass<C, B>();

	bool a = chaos::ClassTools::IsClassDeclared<A>();
	bool b = chaos::ClassTools::IsClassDeclared<B>();
	bool c = chaos::ClassTools::IsClassDeclared<C>();
	bool d = chaos::ClassTools::IsClassDeclared<D>();

	int i1 = chaos::ClassTools::InheritsFrom<A, A>();
	int i2 = chaos::ClassTools::InheritsFrom<A, B>();
	int i3 = chaos::ClassTools::InheritsFrom<B, A>();
	int i4 = chaos::ClassTools::InheritsFrom<C, B>();
	int i5 = chaos::ClassTools::InheritsFrom<C, A>();
	int i6 = chaos::ClassTools::InheritsFrom<A, C>();
	int i7 = chaos::ClassTools::InheritsFrom<A, D>();
	int i8 = chaos::ClassTools::InheritsFrom<D, A>();

#endif

  chaos::WinTools::PressToContinue();

  return 0;
}



