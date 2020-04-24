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


CHAOS_GENERATE_HAS_CALLABLE_FUNCTION(toto)


class A
{
public:

	float toto() const { return 1.3f; }

	int toto(int) { return 2; }


	int toto(float) { return 3; }

	A * toto(A*, A*) const { return nullptr; }


};

class B
{

};



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

	typeof_callable2_toto<A> ppp;

	typeof_callable2_toto<A, int> xxx;

	typeof_callable2_toto<A, A *, A*> yyy;

	bool a1 = has_callable2_toto_v<A>;
	bool a2 = has_callable2_toto_v<A, int>;
	bool a3 = has_callable2_toto_v<A, float>;
	bool a4 = has_callable2_toto_v<A, double>;
	bool a5 = has_callable2_toto_v<A, A>;




	return 0;
}
