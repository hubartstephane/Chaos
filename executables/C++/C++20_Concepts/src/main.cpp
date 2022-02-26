#include <chaos/Chaos.h>

template<typename FUNC, typename CONTAINER>
void Combine(FUNC func, CONTAINER container)
{
	for (auto& value : container)
		func(value);
}




template<size_t CONTAINER_INDEX, typename FUNC, typename ...CONTAINERS, typename ...VALUES>
auto constexpr CombineHelper(FUNC func, std::tuple<CONTAINERS & ...>& containers, VALUES & ... values)
{
	if constexpr (CONTAINER_INDEX < sizeof...(CONTAINERS))
	{
		for (auto& value : std::get<CONTAINER_INDEX>(containers))
		{
			CombineHelper<CONTAINER_INDEX + 1>(func, containers, values..., value);
		}
	}
	else
	{
		func(values...);
	}
}

template<typename FUNC, typename ...PARAMS>
auto constexpr Combine(FUNC func, PARAMS & ... params)
{
	std::tuple<PARAMS & ...> containers = { params... }; // zip all containers into a tuple
	return CombineHelper<0>(func, containers);
}

// -------------------------------------------------------

bool Stop()
{
	return false;

}

bool Stop(bool result)
{
	return result;

}

template<int COUNT, typename FUNC, typename IT, typename ...VALUES>
auto constexpr AllCombinaisonHelper(FUNC func, IT it, IT end, VALUES & ...values)
{
	if constexpr (COUNT > sizeof...(VALUES))
	{
		while (it != end)
		{
			auto tmp = it;
			AllCombinaisonHelper<COUNT>(func, ++it, end, values..., *tmp);
		}
	}
	else
	{
		func(values...);
	}
}


template<int COUNT, typename FUNC, typename CONTAINER>
auto constexpr AllCombinaison(FUNC func, CONTAINER& container)
{
	return AllCombinaisonHelper<COUNT>(func, container.begin(), container.end());
}

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

int CHAOS_MAIN(int argc, char** argv, char** env)
{
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
#if 1
	AllCombinaison<3>([](int a, int b, int c)
		{
			a = a;


		}, v1);
#endif

	std::vector<int> v2 = { 10, 20, 30 };
	std::vector<int> v3 = { 100, 200, 300 };

	Combine([](auto a, auto b, auto c)
		{
			auto sum = a + b + c;
			sum = sum;

		}, v1, v2, v3);

	return 0;
}
