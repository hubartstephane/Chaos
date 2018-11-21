#include <chaos/StandardHeaders.h> 

#include <death/Death.h> 

#include "Ludum41Game.h"

class A
{
public:

	int FF();


};

class B
{
public:

//	int FF(int);


};


#if 0
template<typename T>
int * FFF(T)
{
	return nullptr;
}

template<typename T, typename RET>
char FFF(RET(T::*)())
{
	return 1;
}
template<typename T, typename RET, typename PARAM1 >
char FFF(RET(T::*)(PARAM1))
{
	return 1;
}
#endif



template<typename T, typename P = &T::FF>
char HasFF()
{

	return 7;
}

template<typename T>
int HasFF()
{
	return 6;
}



#if 0

template<typename T>
int * FFF()
{
	return nullptr;
}

template<typename T>
char FFF<RET(T::*)()>()
{
	return 1;
}

template<typename T, typename RET, typename PARAM1 >
char FFF<RET(T::*)(PARAM1)>()
{
	return 1;
}
#endif


int _tmain(int argc, char ** argv, char ** env)
{

	auto x = HasFF<A>();
	auto y = HasFF<B>();

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


