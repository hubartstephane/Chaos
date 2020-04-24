#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>

#include <chaos/Metaprogramming.h>
#include <chaos/EmptyClass.h>
#include <chaos/NamedObject.h>
#include <chaos/ClassTools.h>





CHAOS_GENERATE_HAS_CALLABLE_FUNCTION(toto);

class A
{
public:

	int toto() const { return 1; }

	int toto(int) { return 2; }


	int toto(float) { return 3; }

	int toto(A*, A*) const { return 4; }


};

class B
{

};



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	bool a1 = has_callable2_toto_v<A>;
	bool a2 = has_callable2_toto_v<A, int>;
	bool a3 = has_callable2_toto_v<A, float>;
	bool a4 = has_callable2_toto_v<A, double>;
	bool a5 = has_callable2_toto_v<A, A>;




	return 0;
}
