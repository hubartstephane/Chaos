#include <chaos/Chaos.h>

#include <coroutine>


#if 0

// =====================================
template<typename T, typename G, typename ...BASES>
struct promise_type_base : public BASES...
{
	T mValue;

	auto yield_value(T value)
	{
		mValue = std::move(value);

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

class UU
{
public:

	UU(int i) :value(i) {}

	UU() = default;

	int value = 0;
};

class AA {};

template<typename T>
struct awaitable_promise_type_base
{
	std::optional<T> mRecentSignal;

	struct awaiter
	{
		std::optional<T> & mRecentSignal;

		bool await_ready()
		{
			return mRecentSignal.has_value();
		}

		void await_suspend(std::coroutine_handle<>)
		{
		}

		T await_resume()
		{
			assert(mRecentSignal.has_value());
			auto tmp = *mRecentSignal;
			mRecentSignal.reset();
			return tmp;
		}
	};



	[[nodiscard]] awaiter await_transform(UU uu)
	{
		return awaiter{ mRecentSignal };
	}

	[[nodiscard]] awaiter await_transform(AA, int)
	{
		return awaiter{ mRecentSignal };
	}

};

// =====================================

template<typename T, typename U>
struct [[nodiscard]] async_generator
{
	using promise_type = promise_type_base<
		T,
		async_generator,
		awaitable_promise_type_base<U>>;

	using PromiseTypeHandle = std::coroutine_handle<promise_type>;

	T operator ()()
	{
		auto tmp{ std::move(mCoroHdl.promise().mValue) };

		mCoroHdl.promise().mValue.clear();

		return tmp;
	}

	
	void SendSignal(U signal)
	{
		mCoroHdl.promise().mRecentSignal = signal;
		if (!mCoroHdl.done())
			mCoroHdl.resume();
	}

	async_generator(async_generator const&) = delete;

	async_generator(async_generator&& rhs) :
		mCoroHdl{ std::exchange(rhs.mCoroHdl, nullptr) }
	{
	}

	~async_generator()
	{
		if (mCoroHdl)
			mCoroHdl.destroy();

	}


//private:

	friend promise_type;

	explicit async_generator(promise_type* p) :
		mCoroHdl(PromiseTypeHandle::from_promise(*p))
	{}

	PromiseTypeHandle mCoroHdl;
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



	using iterator = coro_iterator::iterator<promise_type>;

	iterator begin() { return {mCoroHdl}; }
	iterator end() { return {}; }







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

	//explicit
		generator(promise_type * p):
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
		auto p = this; // it compiles, but this is invalid after the co_yield

		while (start < end)
		{
			co_yield start;	
			++start;
		}

	}

	int i = 0;

};



void Test1()
{
	A a(4);

	auto c = a.counter(3, 5);

	int x = c.mCoroHdl.promise().mValue;


	c.mCoroHdl.resume();

	x = c.mCoroHdl.promise().mValue;

	c.mCoroHdl.resume();

	x = c.mCoroHdl.promise().mValue;

	c.mCoroHdl.resume();

	x = c.mCoroHdl.promise().mValue;

#if 0
	c.mCoroHdl.resume();

	for (auto i : c)
	{
		i = i;
	}
#endif
}


using FSM = async_generator<std::string, byte>;

auto dble(auto p)
{
	return  p * p;
}

generator<byte> sender(std::vector<byte> v)
{
	for (auto b : v)
		co_yield dble(b);
}

FSM Parse()
{
	byte b = co_await 3;
	co_yield "toto";
	b = co_await UU{};
	co_yield "toto2";
	b = co_await (AA{}, 1);
	co_yield "toto3";



};



FSM Parse2(DataStreamReader& stream)
{
	while (true)
	{
		byte 

	}
}











#if 1
void ProcessString(generator<byte>& stream, FSM& parse)
{
	for (auto b : stream)
	{
		parse.SendSignal(b);

		if (const auto& res = parse(); res.length())
		{
			int i = 0;
			++i;
		}

	}

}
#endif

int CHAOS_MAIN(int argc, char** argv, char** env)
{
//Test1();

#if 1


	auto s = sender({5,1,3,7,9,8});

	auto p = Parse();

	ProcessString(s, p);


	generator<byte> g = sender({ 1, 4, 8 });

	

#else


	generator<int> g = ([]()
	{
		co_yield 1;
		co_yield 3;
		co_yield 5;
	});

	




#endif

	
	for (auto i : g)
	{
		//i = i;
	}


	return 0;
}

#endif

// ====================================================================

template<typename T, typename G, bool InitialSuspend>
struct promise_type_base
{
	T mValue;

	std::suspend_always yield_value(T value)
	{
		mValue = value;
		return {};
	}

	auto initial_suspend()
	{
		if constexpr (InitialSuspend)
			return std::suspend_always{};
		else
			return std::suspend_never{};
	}

	std::suspend_always final_suspend() noexcept { return {}; }
	G get_return_object() { return G(this); }
	void unhandled_exception();
	void return_void();

};


class DataStreamReader
{

public:

	DataStreamReader() = default;

	DataStreamReader& operator=(DataStreamReader&&) noexcept = delete;

	// ---
	struct Awaiter
	{
		Awaiter& operator = (Awaiter&&) noexcept = delete;

		Awaiter(DataStreamReader& event) noexcept:
			mEvent(event)
		{
			mEvent.mAwaiter = this;
		}

		void await_suspend(std::coroutine_handle<> coroHdl)
		{
			mCoroHdl = coroHdl;
		}

		byte await_resume() noexcept
		{
			assert(mEvent.mData.has_value());
			return *std::exchange(mEvent.mData, std::nullopt);
		}

		void resume()
		{
			mCoroHdl.resume();
		}

	private:

		DataStreamReader& mEvent;
		std::coroutine_handle<> mCoroHdl{};

	};
	// ---

	auto operator co_await() noexcept
	{
		return Awaiter(*this);
	}

	void set(byte b)
	{
		mData.emplace(b);
		if (mAwaiter)
			mAwaiter->resume();
	}

private:

	friend struct Awaiter;

	Awaiter* mAwaiter{};

	std::optional<byte> mData;
};




template<typename T, bool InitialSuspend = true>
class generator
{
	using promise_type = promise_type_base<
		T,
		generator,
		InitialSuspend>;

	using PromiseTypeHandle = std::coroutine_handle<promise_type>;
	
public:

	generator(generator const&) = delete;

	generator(generator&& rhs) :
		mCoroHdl(std::exchange(rhs.mCoroHdl, nullptr)) {}

	~generator()
	{
		if (mCoroHdl)
			mCoroHdl.destroy();
	}

	T operator()
	{
		T tmp{};
		std::swap(tmp, mCoroHdl.promise().mValue);
		return tmp;
	}

private:

	friend promise_type;
	
	explicit generator(promise_type* p):mCoroHdl(PromiseTypeHandle::from_promise(*p))
	{

	}

protected:

	PromiseTypeHandle mCoroHdl;


};

using FSM = generator<std::string, false>;


DataStreamReader GetReader(std::vector<byte>& v)
{
	for (auto b : v)
		co_yield b;
}

FSM Parse(DataStreamReader& reader)
{
	while (true)
	{
		byte b = co_await reader;

		co_yield "toto";

	}


}




int CHAOS_MAIN(int argc, char** argv, char** env)
{


	return 0;
}