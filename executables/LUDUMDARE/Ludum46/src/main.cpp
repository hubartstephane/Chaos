#include <chaos/StandardHeaders.h> 


#include <chaos/DataOwner.h> 

#include <death/Death.h> 


#include "Ludum46Game.h"


// XXX : on peut faire 
//
//   dynamic_cast<T>(C)    a partir du moment ou C a une VTable, pas necessairement T

class B
{
	//virtual void f() {}

};

class A : public B, public chaos::DataOwner<int>
{
public:

	A() : chaos::DataOwner<int>(666) {}
	

};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	B* b = new A;

	chaos::DataOwner<int>* d = chaos::auto_cast(b);


	int* p = chaos::GetOwnedData<int>(b);




	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
