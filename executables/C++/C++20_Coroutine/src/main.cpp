#include <chaos/Chaos.h>

#include <coroutine>


// =====================================
template<typename T, typename G>
struct promise_type_base
{
	T mValue = 666;

	float fValue = 666.6f;

	auto yield_value(T value)
	{
		mValue = std::move(value);

		return std::suspend_always{};
	}

	auto yield_value(float value)
	{

		fValue = std::move(value);

		return std::suspend_always{};

	}

	G get_return_object() 
	{ 
		return G(this); 
	}


	std::suspend_always initial_suspend() { return {}; }

	std::suspend_always final_suspend() noexcept { return {}; }

	void return_void() {}

	void unhandled_exception() 
	{ 
		std::terminate(); 
	}
#if 0
	static auto get_return_object_on_allocation_failure()
	{
		return G(nullptr);
	}
#endif

};


// =====================================

namespace coro_iterator
{
	template<typename PT>
	struct iterator
	{
		std::coroutine_handle<PT> mCoroHdl{ nullptr };

		void resume()
		{
			if (!mCoroHdl.done())
			{
				mCoroHdl.resume();
			}
		}

		iterator() = default;

		iterator(std::coroutine_handle<PT> hco) :mCoroHdl(hco)
		{

			resume();
		}

		void operator ++()
		{
			resume();
		}

		bool operator == (const iterator&) const
		{
			return mCoroHdl.done();
		}

		const auto& operator *() const
		{
			return mCoroHdl.promise().mValue;
		}
	};
};


// =====================================

template<typename T>
struct generator
{
	using promise_type = promise_type_base<T, generator>;

	using PromiseTypeHandle = std::coroutine_handle<promise_type>;



	//using iterator = coro_iterator::iterator<promise_type>;

	//iterator begin() { return {mCoroHdl}; }
	//iterator end() { return {}; }







	generator(generator const&) = delete;

	generator(generator&& rhs) :mCoroHdl(std::exchange(rhs.mCoroHdl, nullptr)) 
	{}

	~generator()
	{
		if (mCoroHdl)
			mCoroHdl.destroy();
	}

private:
public:

	friend promise_type;

	explicit generator(promise_type * p):
		mCoroHdl(PromiseTypeHandle::from_promise(*p))
	{}

	PromiseTypeHandle mCoroHdl;
};

// =====================================

using IntGenerator = generator<int>;

class A
{
public:

	A(int ii): i(ii) {}

	IntGenerator counter(int start, int end)
	{
		auto p = this;

		while (start < end)
		{
			//		co_yield start;	

			co_yield (float)start;
			++start;
		}

	}

	int i = 0;

};






int CHAOS_MAIN(int argc, char** argv, char** env)
{
	A a(4);

	auto c = a.counter(3, 5);

	int x = c.mCoroHdl.promise().mValue;
	float y = c.mCoroHdl.promise().fValue;


	c.mCoroHdl.resume();

	x = c.mCoroHdl.promise().mValue;
	y = c.mCoroHdl.promise().fValue;

	c.mCoroHdl.resume();

	x = c.mCoroHdl.promise().mValue;
	y = c.mCoroHdl.promise().fValue;

	c.mCoroHdl.resume();

	x = c.mCoroHdl.promise().mValue;
	y = c.mCoroHdl.promise().fValue;

#if 0
	c.mCoroHdl.resume();

	for (auto i : c)
	{
		i = i;
	}
#endif



	return 0;
}