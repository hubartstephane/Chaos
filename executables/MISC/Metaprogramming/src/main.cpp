#include <chaos/Chaos.h>

CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(toto)


class A
{
public:

	char toto() const { return 0; }

	char toto(int) { return 0; }

	char toto(float) { return 0; }

	char toto(A*, A*) const { return 0; }

};

class B
{

};

int toto(A* a)
{
	return 666;
}

float toto(B* b)
{
	return 666.0f;
}

B* toto()
{
	return nullptr;
}

A* toto(int i)
{
	return nullptr;
}





int main(int argc, char ** argv, char ** env)
{


	bool a1 = check_method_toto_v<A>;
	bool a2 = check_method_toto_v<A, int>;
	bool a3 = check_method_toto_v<A, float>;
	bool a4 = check_method_toto_v<A, double>;
	bool a5 = check_method_toto_v<A, A>;
	bool a6 = check_method_toto_v<A, A*, A*>;

	bool b1 = check_method_toto_v<A const>;
	bool b2 = check_method_toto_v<A const, int>;
	bool b3 = check_method_toto_v<A const, float>;
	bool b4 = check_method_toto_v<A const, double>;
	bool b5 = check_method_toto_v<A const, A>;
	bool b6 = check_method_toto_v<A const, A*, A*>;

	typeof_method_toto<A> t1;
	typeof_method_toto<A, int> t2;
	typeof_method_toto<A, A*, A*> t3;



	bool c1 = check_function_toto_v<A*>;
	bool c2 = check_function_toto_v<B*>;
	bool c3 = check_function_toto_v<int>;
	bool c4 = check_function_toto_v<int, int>;
	bool c5 = check_function_toto_v<A const *>;
	bool c6 = check_function_toto_v<>;

	typeof_function_toto<A *> u1;
	typeof_function_toto<B *> u2;
	typeof_function_toto<> u3;
	typeof_function_toto<int> u4;


	return 0;
}
