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


  {

    chaos::NamedObjectVector<A>  vector1;
    vector1.elements.push_back(A());
    A* a1 = vector1.Find(666);
    A* a2 = vector1.Find("toto");

    chaos::NamedObjectVector<A*> vector2;
    vector2.elements.push_back(new A());
    A* a3 = vector2.Find(666);
    A* a4 = vector2.Find("toto");

  }

	chaos::TextureAtlasx::FontAtlasFontParams entry;
	entry = entry;

	//vector2.Find(666);




#if 0
	chaos::meta::remove_all_pointer<int ****>::type iii = 666;


  //delete(new MyClass<MyTransf>);

  std::cout << "======================" << std::endl;

	delete(new T);
 

	IsTrue(chaos::has_logger_tag<chaos::logger>::type());

#endif


	chaos::Application::Finalize();
	return 0;
}


