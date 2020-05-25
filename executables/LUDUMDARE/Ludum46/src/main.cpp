#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 


#include <chaos/Class.h> 

#include "Ludum46Game.h"



#define DECLARE_CLASS(classname)\
private:\
static inline chaos::Class * classname##_register = chaos::ClassTools::DeclareClass<classname>(#classname);\
public:\
static chaos::Class * GetStaticClass(){ return classname##_register;}\
virtual chaos::Class * GetClass() { return classname##_register; }

class A
{
public:

	DECLARE_CLASS(A);

};

class B : public A
{
public:

	DECLARE_CLASS(B);

};




int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
