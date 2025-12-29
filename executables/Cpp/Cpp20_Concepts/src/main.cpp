#include "chaos/Chaos.h"

template<typename T>
concept is_int = requires(T t)
{
	(3 + 6);

	(t + t);

	//t.begin();

		requires std::same_as<T, int>;
};

class A
{

public:

	int fff(int k)
	{
		return k * k;
	}
};

class F
{
public:


	void operator () (int)
	{
		//return 668;
	}

private:

	void fff(int k) {}

};

template<typename T>
concept CanCallFFF = requires(T t)
{
	t.fff(666);

};


class B
{
public:

	void operator()() {}
};

class C
{
public:

	int operator()() { return 0; }
};






auto identity(is_int auto i)
{
	return i;
}

auto identity(auto i)
{
	return 666;
}

auto sum(auto ...args)
{
	return (... + args);
}

template<typename T>
concept ReturnInt = requires(T t)
{
	{t()} -> std::same_as<int>;
};


template<typename T>
concept SupportValidation = requires(T t)
{
	t.validate();
};

class V
{
public:

	void validate() {}
};



class D
{
public:

	~D() {}
};

class E
{
public:

	virtual ~E() {}
};

class G
{
public:


};

class H
{
public:

	E e;
};

class I : public D
{
public:


};

template<typename T>
class J
{
public:



	~J() requires (std::same_as<T, int>)
	{

	}

	~J() = default;
};


template<std::random_access_iterator T>
void myfunc(T const& t)
{
	int i = 0;
	++i;
}

template<typename T>
void myfunc(T const& t)
{
	int i = 0;
	++i;
}


template<typename ...ARGS>
void MLOG(ARGS ...args)
{
	(std::clog << ... << args);

}


int main(int argc, char** argv, char** env)
{
	chaos::WinTools::AllocConsoleAndRedirectStdOutput();
	MLOG("toto", 3, 9, 5.0f, "titi");


	auto a = identity(3);
	auto b = identity(A());
	auto c = sum(1, 2, 3, 4, 5);
	bool d = std::invocable<A>;
	bool e = std::invocable<F>;
	bool f = std::invocable<F, int>;

	bool g = CanCallFFF<A>;
	bool h = CanCallFFF<F>;


	//std::invoke(F(), 3);

	auto ptr = &A::fff;
	auto i = std::invoke(ptr, A(), 15);

	bool j = ReturnInt<B>;
	bool k = ReturnInt<C>;


	std::integral auto myvalue1 = 666; // on utilise auto et peut verifier/indique qu on aura bien un type integral
	//std::integral auto myvalue2 = 666.0f;



	bool l = SupportValidation<V>;
	bool m = SupportValidation<V const>;

	V const v;
	if constexpr (SupportValidation<decltype(v)>)
	{

		// v.validate(); // on devrait pouvoir compiler avec cette ligne decommente
						 // car on a
						 // m == false     +    constexpr
						 // ce code devrait etre elimine
	}


	auto n = std::is_copy_constructible_v<A>;
	auto o = std::is_default_constructible_v<A>;
	auto p = std::is_trivially_destructible_v<A>;

	auto q = std::is_trivially_destructible_v<D>;
	auto r = std::is_trivially_destructible_v<E>;
	auto s = std::is_trivially_destructible_v<G>;
	auto t = std::is_trivially_destructible_v<H>;
	auto u = std::is_trivially_destructible_v<I>;


	auto w = std::is_trivially_destructible_v<J<int>>;
	auto x = std::is_trivially_destructible_v<J<float>>;

	auto y = std::random_access_iterator<std::vector<int>>;
	auto z = std::random_access_iterator<std::list<int>>;

	myfunc(std::vector<int>());
	myfunc(std::list<int>());


	std::vector<int> v1 = { 1, 2, 3, 4, 5, 6 };

	std::vector<int> v2 = { 10, 20, 30 };
	std::vector<int> v3 = { 100, 200, 300 };

	return 0;
}
