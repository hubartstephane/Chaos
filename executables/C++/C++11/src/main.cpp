#include <iostream>
#include <memory>
#include <vector>
#include <ranges>
#include <numeric>



class A
{
public:

	A(int ii) :i(ii) {}

	int i = 0;
};

bool operator == (A const& src1, A const& src2)
{
	return (src1.i == src2.i);
}

int main(int argc, char ** argv, char ** env)
{
	bool b1 = (A(3) == A(5));
	bool b2 = (A(3) != A(5));



	return 0;
}