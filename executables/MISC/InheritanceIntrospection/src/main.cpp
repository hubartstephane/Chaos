#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/ClassTools.h>

class A {};

class B : public A {};

class C : public B {};

class D {};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	chaos::ClassTools::DeclareClass<A>();
	chaos::ClassTools::DeclareClass<B, A>();
	chaos::ClassTools::DeclareClass<C, B>();

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

  chaos::WinTools::PressToContinue();

  return 0;
}


