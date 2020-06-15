#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/Class.h>
#include <chaos/Object.h>

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
	SubClassOf() = default;
	/** constructor */
	SubClassOf(SubClassOf const& src) = default;
	/** constructor */
	SubClassOf(chaos::Class const * src)
	{
		chaos::Class const* base_class = chaos::Class::FindClass<T>();
		if (base_class == nullptr)
		{
			base_class = base_class;
		}
		else if (src->InheritsFrom(base_class, true) != chaos::InheritanceType::YES)
		{


			base_class = base_class;
		}
		else
		{
			base_class = base_class;
		}	
	}


	/** constructor */
	template<typename U>
	SubClassOf(SubClassOf<U> const& src)
	{
		static_assert(std::is_base_of_v<T, U>);
		internal_class = src.GetInternalClass();
	}

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


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

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



