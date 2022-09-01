#include <iostream>
#include <memory>
#include <vector>



class A
{
public:

	std::string GetName1() const { return name; }

	std::string const & GetName2() const { return name; }

	std::string name;
};

class B
{
public:
	std::vector<std::unique_ptr<A>> myA;
};

bool operator == (A const&, A const&)
{
	return true;
}


int main(int argc, char ** argv, char ** env)
{
	std::vector<std::unique_ptr<A>> myA;

	myA.emplace_back(new A);


	A a;

	bool b = (a != a);

	auto s1 = a.GetName1();
	s1 = "t";
	auto s2 = a.GetName2();
	s2 = "t";

	decltype(auto) s3 = a.GetName1();
	s3 = "t";

	decltype(auto) s4 = a.GetName2();
	//s4 = "t";


	std::cout << "Hello World Again" << std::endl;

	return 0;
}