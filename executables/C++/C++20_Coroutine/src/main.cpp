#include <chaos/Chaos.h>

#include <coroutine>



template<typename RET_TYPE = void, bool START_SUSPENDED = true>
class Task;

//////////////////////////////////////////

template<typename RET_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromise
{
public:

	bool HasYieldValue() const
	{
		return y_value.has_value();
	}

	RET_TYPE const& GetYieldValue() const
	{
		assert(HasYieldValue());
		return y_value.value();
	}

	bool HasReturnValue() const
	{
		return r_value.has_value();
	}

	RET_TYPE const& GetReturnValue() const
	{
		assert(HasReturnValue());
		return r_value.value();
	}

	auto yield_value(RET_TYPE value)
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

	void return_void()
	{
		int i = 3;
		++i;

	}
#if 0
	void return_value(int value)
	{
		r_value = std::move(value);
		int i = 3;
		++i;
	}

	void return_value(char const * value)
	{
		int i = 3;
		++i;
	}
#endif
	void unhandled_exception()
	{
		int i = 3;
		++i;
	}








	struct Awaiter
	{
		bool await_ready()
		{
			return false;
		}

		void await_suspend(std::coroutine_handle<> p)
		{
			int i = 0;
			++i;
		}

		int await_resume()
		{
			return 666;
		}
	};




	template<typename OTHER_RET_TYPE, bool OTHER_START_SUSPENDED>
	Awaiter await_transform(Task<OTHER_RET_TYPE, OTHER_START_SUSPENDED>& other_task)
	{
		return {};
	}









protected:

	std::optional<RET_TYPE> y_value;

	std::optional<RET_TYPE> r_value;
};


//////////////////////////////////////////


template<typename RET_TYPE, bool START_SUSPENDED>
class Task
{

public:

	using promise_type = TaskPromise<RET_TYPE, START_SUSPENDED, Task>;

	using promise_handle = std::coroutine_handle<promise_type>;

	friend class promise_type;



	~Task()
	{
		if (handle)
			handle.destroy();
	}

	void Resume()
	{
		handle.resume();
	}

	bool IsDone() const
	{
		return handle.done();
	}

	bool HasYieldValue() const
	{
		return handle.promise().HasYieldValue();
	}

	bool HasReturnValue() const
	{
		return handle.promise().HasReturnValue();
	}

















protected:

	explicit Task(promise_type* p) :
		handle(promise_handle::from_promise(*p)) {}

protected:

	promise_handle handle;

};


Task<int> Generates()
{
	co_yield 1;
	co_yield 2;
	co_yield 3;
	co_yield 4;
}

Task<int> MyTask(int value, Task<int> & getter)
{
	for (int i = 0; i < 5; ++i)
	{

		int x = co_await getter;

		co_yield (3 * value);
		co_yield (3 * value);
	}
}



int CHAOS_MAIN(int argc, char** argv, char** env)
{
	Task<int> g = Generates();

	Task<int> f = MyTask(6, g);


	while (!f.IsDone())
	{
		f.Resume();

	}



#if 0

	Task<int> t = MyTask(666);
	bool a1 = t.HasYieldValue();
	bool b1 = t.HasReturnValue();
	bool c1 = t.IsDone();
	t.Resume();
	bool a2 = t.HasYieldValue();
	bool b2 = t.HasReturnValue();
	bool c2 = t.IsDone();
	t.Resume();
	bool a3 = t.HasYieldValue();
	bool b3 = t.HasReturnValue();
	bool c3 = t.IsDone();
	t.Resume();
	bool a4 = t.HasYieldValue();
	bool b4 = t.HasReturnValue();
	bool c4 = t.IsDone();



#endif

	return 0;
}