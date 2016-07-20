#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/InheritanceIntrospection.h>

class A {};
class B {};
class C {};

template<typename BASE, typename PARENT>
void TestInheritance(char const * base_name, char const * parent_name)
{
  chaos::InheritanceIntrospection::ClassRuntimeID base   = chaos::InheritanceIntrospection::GetClassRuntimeID<BASE>();
  chaos::InheritanceIntrospection::ClassRuntimeID parent = chaos::InheritanceIntrospection::GetClassRuntimeID<PARENT>();

  bool inherits = chaos::InheritanceIntrospection::InheritsFrom(base, parent);

  std::cout << base_name << " inherits from " << parent_name << " ?  ==> " << inherits << std::endl;
}

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  chaos::InheritanceIntrospection::DeclareInheritance<A, B>();
  chaos::InheritanceIntrospection::DeclareInheritance<B, C>();

  TestInheritance<A, B>("a", "b");
  TestInheritance<A, C>("a", "c");
  TestInheritance<B, C>("b", "c");

  TestInheritance<B, A>("b", "a");
  TestInheritance<C, A>("c", "a");
  TestInheritance<C, B>("c", "b");

  chaos::WinTools::PressToContinue();

  return 0;
}


