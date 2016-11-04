#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>

#include <chaos/Metaprogramming.h>
#include <chaos/EmptyClass.h>
#include <chaos/NamedObject.h>
#include <chaos/NamedObjectVector.h>

#include <chaos/TextureAtlas.h>
#include <chaos/TextureAtlasGenerator.h>


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


class T : public chaos::BaseClass<chaos::logger> {};

//class T : public chaos::add_logger<> {};


//class T : public cond_add_logger<has_logger_tag<boost::mpl::_1>, boost::mpl::_2>

//using TAGS = boost::mpl::vector<chaos::logger>;

//class T : public chaos::cond_add_logger<chaos::has_logger_tag<TAGS>, chaos::EmptyClass> {};

class A : public chaos::NamedObject
{

};

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();


	chaos::Application::Finalize();
	return 0;
}


