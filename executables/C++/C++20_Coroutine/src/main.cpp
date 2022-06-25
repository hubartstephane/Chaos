#include <chaos/Chaos.h>

#include <coroutine>



template<typename YIELD_TYPE = void, typename RETURN_TYPE = void, bool START_SUSPENDED = true>
class Task;

//////////////////////////////////////////

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromise
{
public:

	auto yield_value(YIELD_TYPE value)
	{
		y_value = std::move(value);
		return std::suspend_always {};
	}

	TASK_TYPE get_return_object()
	{
		return TASK_TYPE(this);
	}

	auto initial_suspend()
	{
		if constexpr (START_SUSPENDED)
			return std::suspend_always{};
		else
			return std::suspend_never{};
	}

	auto final_suspend() noexcept
	{
		return std::suspend_always{};
	}
#if 0

	void return_void() requires(std::is_same_v<RETURN_TYPE, void>)
	{
	}

#else

	template<typename T>
	void return_value(T t)
	{
		t = t;
	}

	void return_value(RETURN_TYPE result) requires (!std::is_same_v<RETURN_TYPE, void>)
	{
		r_value = std::move(result);
	}
#endif

	void unhandled_exception()
	{
	}





	template<typename OTHER_YIELD_TYPE, typename OTHER_RETURN_TYPE, bool OTHER_START_SUSPENDED>
	struct Awaiter
	{
		Awaiter(Task<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED>& in_other_task):
			other_task(in_other_task)
		{

		}

		bool await_ready()
		{
			return false;
		}

		auto await_suspend(std::coroutine_handle<> in_p /* the coroutine about to be suspended */)
		{
			int i = 0;
			++i;

			p = in_p;

			return other_task.handle; // enter other Task
		}

		auto await_resume()
		{
			return other_task.GetYieldValue();
		}

		Task<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED> & other_task;

		std::coroutine_handle<> p = nullptr;
	};




	template<typename OTHER_YIELD_TYPE, typename OTHER_RETURN_TYPE, bool OTHER_START_SUSPENDED>
	Awaiter<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED> await_transform(Task<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED>& other_task)
	{
		return { other_task };
	}




	std::optional<YIELD_TYPE> y_value;

	std::optional<RETURN_TYPE> r_value;
};


//////////////////////////////////////////


template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED>
class Task
{

public:

	using promise_type = TaskPromise<YIELD_TYPE, RETURN_TYPE, START_SUSPENDED, Task>;

	using promise_handle = std::coroutine_handle<promise_type>;

	friend class promise_type;

	~Task()
	{
		if (handle)
			handle.destroy();
	}

	/** resume the coroutine */
	void Resume()
	{
		handle.resume();
	}

	/** check whether coroutine is completed */
	bool IsDone() const
	{
		return handle.done();
	}

	/** check whether coroutine has a yielded value */
	bool HasYieldValue() const
	{
		return handle.promise().y_value.has_value();
	}

	/** check whether coroutine has a returned value */
	bool HasReturnValue() const
	{
		return handle.promise().r_value.has_value();
	}

	/** read yielded value + destruction */
	YIELD_TYPE GetYieldValue() const
	{
		assert(HasYieldValue());
		YIELD_TYPE result = std::move(*handle.promise().y_value);
		handle.promise().y_value.reset();
		return result;
	}

	/** read yielded value without destroying it */
	YIELD_TYPE PeekYieldValue() const
	{
		assert(HasYieldValue());
		return *handle.promise().y_value;
	}

	/** read returned value*/
	RETURN_TYPE GetReturnValue() const
	{
		assert(HasReturnValue());
		return *handle.promise().r_value;
	}

protected:

	/** constructor */
	explicit Task(promise_type* p) :
		handle(promise_handle::from_promise(*p)) {}

public:

	promise_handle handle;

};

class A
{
public:

	int f()
	{
		return 8;
	}

	Task<int, int> Generates()
	{
		auto X = this;

		co_yield f();

		auto Y = this;

		f();

		co_yield 9;
		co_yield 13;
		co_yield 17;

		co_return 3;
	}

	Task<int, int> MyTask(int value, Task<int, int>& getter)
	{
		auto X = this;
		 
		for (int i = 0; i < 3; ++i)
		{

			int x = co_await getter;


			auto Y = this;

			f();

			co_yield (3 * x);
			co_yield (3 * value);
		}

		float xx = chaos::MathTools::RandFloat();
		if (xx > 0.5f)
			co_return *this;
		co_return 3;
	}


	int XXX = 666;

};

auto operator co_await (A a)
{
	return std::suspend_always{};
}


int CHAOS_MAIN(int argc, char** argv, char** env)
{
	A a;

	chaos::MathTools::ResetRandSeed();

	Task<int, int> g = a.Generates();

	Task<int, int> f = a.MyTask(6, g);


	while (!f.IsDone())
	{
		f.Resume();

		a.f();
		//int a = f.GetYieldValue();

		argc = argc;

	}

	return 0;
}