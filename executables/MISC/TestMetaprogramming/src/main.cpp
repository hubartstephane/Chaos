#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>

#include <chaos/Metaprogramming.h>
#include <chaos/EmptyClass.h>




/*


template<typename T1, typename T2>
using A = boost::mpl::inherit_linearly<
	boost::mpl::vector<T1, T2>,
	boost::mpl::inherit< 
		boost::mpl::_1, 
		boost::mpl::identity<boost::mpl::_2> 
	> 
>;

*/

/***********************************************************************************************/

bool IsTrue(boost::mpl::true_)
{
	return true;
}

bool IsTrue(boost::mpl::false_)
{
	return false;
}

/***********************************************************************************************/


using MyTransf = boost::mpl::vector<
  chaos::add_logger<boost::mpl::_1>,
  chaos::add_logger<boost::mpl::_1>
>;



template<typename BEGIN, typename END, typename BASE>
class MyClassImpl : public MyClassImpl <
  typename boost::mpl::next<BEGIN>::type,
  END,
  typename boost::mpl::apply<
  typename boost::mpl::deref<BEGIN>::type,
    BASE
  >::type
> {};
  

template<typename IT, typename BASE>
class MyClassImpl < IT, IT, BASE > : public BASE {};

template<typename SEQ, typename BASE = chaos::EmptyClass>
using MyClass = MyClassImpl<
  typename boost::mpl::template begin<SEQ>::type,
  typename boost::mpl::template end<SEQ>::type,
  BASE>;


class T : public chaos::BaseClass<> {};


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  delete(new MyClass<MyTransf>);

  std::cout << "======================" << std::endl;

 

	chaos::Application::Finalize();
	return 0;
}


