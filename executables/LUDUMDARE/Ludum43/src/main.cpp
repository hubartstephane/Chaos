#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 
#include "Ludum43Game.h"

#include <chaos/EmptyClass.h>
#include <chaos/MetaProgramming.h>

#define cccCHAOS_GENERATE_HAS_TRAIT(name)\
BOOST_MPL_HAS_XXX_TRAIT_DEF(name)\
template<typename T, typename B = typename has_##name<T>::type>\
class get_##name;\
template<typename T>\
class get_##name<T, boost::mpl::true_> : public boost::mpl::identity<T>\
{};\
template<typename T>\
class get_##name<T, boost::mpl::false_> : public boost::mpl::identity<chaos::EmptyClass>\
{};


cccCHAOS_GENERATE_HAS_TRAIT(toto)

class A
{

};

class B
{
public:

	class toto {};
};

template<typename T>
class C
{
public:

	using tt = typename get_toto<T>::type;


};



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{	
	auto a = has_toto<A>::type();
	auto b = has_toto<B>::type();
	auto c = get_toto<A>::type();
	auto d = get_toto<B>::type();


	auto e = C<A>::tt();
	auto f = C<B>::tt();



	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


