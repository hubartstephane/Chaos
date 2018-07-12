#include <chaos/StandardHeaders.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 

#include "LudumWindow.h"
#include "LudumGame.h"

// ======================================================================================

#if 0
	  { "position",   SEMANTIC_POSITION },
	  { "color",      SEMANTIC_COLOR },
	  { "normal",     SEMANTIC_NORMAL },
	  { "binormal",   SEMANTIC_BINORMAL },
	  { "tangent",    SEMANTIC_TANGENT },
	  { "texcoord",   SEMANTIC_TEXCOORD },
	  { "boneindex",  SEMANTIC_BONEINDEX },
	  { "boneweight", SEMANTIC_BONEWEIGHT },
	  { "userdata",   SEMANTIC_USERDATA },


//BOOST_TTI_HAS_MEMBER_FUNCTION(AMemberFunction)



BOOST_TTI_HAS_MEMBER_DATA(positionX)

BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_position_tag, position, true)




class A
{
public:
	typedef float position;
};

class A2 : public A
{


};

class B
{
public:
	A2 positionX;
};

template<typename T, typename HAS_DATA = has_member_data_positionX<A2>::type>
void FF();

template<typename T>
void FF<T, boost::mpl::false_>()
{

}

template<typename T>
void FF<T, boost::mpl::true_>()
{

}


class P
{
public:

	int x;


};


bool b1 = has_position_tag<A>::value;
bool b2 = has_position_tag<B>::value;

bool b3 = has_member_data_positionX<A, float>::value;
bool b4 = has_member_data_positionX<B, float>::value;
bool b5 = has_member_data_positionX<B, A>::value;
bool b6 = has_member_data_positionX<B, A2>::value;


FF<A>();
FF<B>();

#endif






int _tmain(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<LudumWindow>(argc, argv, env, params);
	return 0;
}


