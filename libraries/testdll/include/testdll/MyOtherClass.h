#include "testdll/Config.h"
#include <string>
#include <memory>
#include <vector>

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4251)
#endif

class TESTDLL_API A
{

};

// https://stackoverflow.com/questions/29565299/how-to-use-a-vector-of-unique-pointers-in-a-dll-exported-class-with-visual-studi
//
// It appears adding __declspec(dllexport) forces the compiler to define the implicitly - declared copy constructor and copy assignment operator (normally, this only happens if they're used). These in turn call the copy constructor/assignment operator of v. But the copy operations of std::vector<T> are ill-formed for a non-copyable T, such as std::unique_ptr. Hence the error.
//
// When the member is just std::unique_ptr, the problem does not occur, because that has copy operations explicitly deletedand so the default copy operations of A become deleted as well.
class TESTDLL_API MyOtherClass
{
public:

	//MyOtherClass(MyOtherClass const&) = delete;

	//MyOtherClass & operator = (MyOtherClass const&) = delete;

	void f();

	std::string name;

	std::vector<std::unique_ptr<A>> myA;

	std::vector<std::shared_ptr<A>> myA2;
};

#ifdef _MSC_VER
#  pragma warning(pop)
#endif