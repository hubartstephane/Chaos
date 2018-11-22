#include <chaos/StandardHeaders.h> 

#include <death/Death.h> 

#include "Ludum41Game.h"

class A
{
public:

	static int FF() { return 0; }


};

class B
{
public:




};


#if 1

char HasFF(...);

template<typename T>
auto HasFF(T const & t) -> decltype(&T::FF);


#endif

#if 0

template<int i>
class Integer
{

};

template<typename T>
class HasFF
{
	int p[10];
};

template<typename T>
class HasFF : public Integer<sizeof(&T::FF)>
{
	int p[100];
};
#endif




int _tmain(int argc, char ** argv, char ** env)
{

//	auto x = sizeof(HasFF<A>);
//	auto y = sizeof(HasFF<B>);

	auto x = sizeof(HasFF(A()));
	auto y = sizeof(HasFF(B()));



	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


