#include "chaos/Chaos.h"

#include <coroutine>



template<typename YIELD_TYPE = void, typename RETURN_TYPE = void, bool START_SUSPENDED = true>
class Task;

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromise;

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskInternal;

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskInternal : public chaos::Object
{
public:

	using task_promise_type = TaskPromise<YIELD_TYPE, RETURN_TYPE, START_SUSPENDED, TASK_TYPE>;
	using promise_handle = std::coroutine_handle<task_promise_type>;

	/** destructor */
	virtual ~TaskInternal()
	{
		if (handle != nullptr)
			handle.destroy();
	}

	/** resume the coroutine */
	void Resume()
	{
		if (handle != nullptr)
			handle.resume();
	}

	/** check whether coroutine is completed */
	bool IsDone() const
	{
		return (handle == nullptr) || (handle.done());
	}

	/** check whether coroutine has a yielded value */
	bool HasYieldValue() const
	{
		return (y_value.has_value());
	}

	/** check whether coroutine has a returned value */
	bool HasReturnValue() const
	{
		return (r_value.has_value());
	}
	/** read yielded value + destruction */
	YIELD_TYPE ConsumeYieldValue() const
	{
		assert(HasYieldValue());
		YIELD_TYPE result = std::move(*y_value);
		y_value.reset();
		return result;
	}

	/** read yielded value without destroying it */
	YIELD_TYPE GetYieldValue() const
	{
		assert(HasYieldValue());
		return *y_value;
	}

	/** read returned value*/
	RETURN_TYPE GetReturnValue() const
	{
		assert(HasReturnValue());
		return *r_value;
	}

public:

	/** the last yielded value */
	std::optional<YIELD_TYPE> y_value;
	/** the return value */
	std::optional<RETURN_TYPE> r_value;
	/** the handle for the coroutine */
	promise_handle handle;
};



/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromise
{
public:

	using task_internal_type = TaskInternal<YIELD_TYPE, RETURN_TYPE, START_SUSPENDED, TASK_TYPE>;
	using task_type = TASK_TYPE;

	/** destructor */
	~TaskPromise()
	{
		if (task_internal != nullptr)
			task_internal->handle = nullptr;
	}
	/** return a value */
	void return_value(RETURN_TYPE result) requires (!std::is_same_v<RETURN_TYPE, void>)
	{
		if (task_internal != nullptr)
			task_internal->r_value = std::move(result);
	}
	/** emit a value */
	auto yield_value(YIELD_TYPE value)
	{
		if (task_internal != nullptr)
			task_internal->y_value = std::move(value);
		return std::suspend_always{};
	}
	/** uncaught exception */
	void unhandled_exception()
	{
	}
	/** whether the task is initially suspended or not */
	auto initial_suspend()
	{
		if constexpr (START_SUSPENDED)
			return std::suspend_always{};
		else
			return std::suspend_never{};
	}
	/** whether the task is finally suspended */
	auto final_suspend() noexcept
	{
		return std::suspend_never{};
	}
	/** create the related Task */
	task_type get_return_object()
	{
		task_internal = new task_internal_type();
		if (task_internal != nullptr)
			task_internal->handle = std::coroutine_handle<TaskPromise>::from_promise(*this);
		return task_type(task_internal.get());
	}

public:

	/** the pointer on the shared internal data */
	chaos::weak_ptr<task_internal_type> task_internal;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED>
class Task
{
public:

	using task_internal_type = TaskInternal<YIELD_TYPE, RETURN_TYPE, START_SUSPENDED, Task>;
	using promise_type = TaskPromise<YIELD_TYPE, RETURN_TYPE, START_SUSPENDED, Task>;

	friend class promise_type;

	/** constructor */
	Task(task_internal_type* in_task_internal = nullptr) :
		task_internal(in_task_internal)
	{
	}
	/** resume the coroutine */
	void Resume()
	{
		assert(task_internal != nullptr);
		task_internal->Resume();
	}
	/** check whether coroutine is completed */
	bool IsDone() const
	{
		return (task_internal != nullptr) && (task_internal->IsDone());
	}
	/** check whether coroutine has a yielded value */
	bool HasYieldValue() const
	{
		return (task_internal != nullptr) && (task_internal->HasYieldValue());
	}
	/** check whether coroutine has a returned value */
	bool HasReturnValue() const
	{
		return (task_internal != nullptr) && (task_internal->HasReturnValue());
	}
	/** read yielded value + destruction */
	YIELD_TYPE ConsumeYieldValue() const
	{
		assert(task_internal != nullptr);
		return task_internal->ConsumeYieldValue();
	}
	/** read yielded value without destroying it */
	YIELD_TYPE GetYieldValue() const
	{
		assert(task_internal != nullptr);
		return task_internal->GetYieldValue();
	}
	/** read returned value*/
	RETURN_TYPE GetReturnValue() const
	{
		assert(task_internal != nullptr);
		return task_internal->GetReturnValue();
	}

protected:

	/** the pointer to the internal data */
	chaos::shared_ptr<task_internal_type> task_internal;
};






















/////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0


#if 0

	void return_void() requires(std::is_same_v<RETURN_TYPE, void>)
	{
	}

#endif



	template<typename OTHER_YIELD_TYPE, typename OTHER_RETURN_TYPE, bool OTHER_START_SUSPENDED>
	struct Awaiter
	{
		Awaiter(Task<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED> const & in_other_task):
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

			//return other_task.handle; // enter other Task
		}

		auto await_resume()
		{
			return other_task.GetYieldValue();
		}

		Task<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED> const & other_task;

		std::coroutine_handle<> p = nullptr;
	};




	template<typename OTHER_YIELD_TYPE, typename OTHER_RETURN_TYPE, bool OTHER_START_SUSPENDED>
	Awaiter<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED> await_transform(Task<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED> const& other_task)
	{
		return { other_task };
	}

#endif






#if 0

Task<int, int> Generator()
{
	int i = 0;
	while (true)
	{
		std::cout << "Generator: about to yield [" << i << "]" << std::endl;
		co_yield ++i;
		std::cout << "Generator: yield done" << std::endl;

	}
	co_return 666;
}

Task<int, int> Func()
{
	while (true)
	{
		std::cout << "Func: about to co_await" << std::endl;
		int result = co_await Generator();
		std::cout << "Func: co_await" << result << std::endl;

	}
	co_return 777;
}
#endif


Task<int, int> Generator()
{
	co_yield 1;
	co_yield 2;
	co_return 5;
}


int main(int argc, char** argv, char** env)
{
	{
		Task<int, int> generator = Generator();

		env = env;
	}

#if 0
	Task<int, int> generator = Generator();

	while (!generator.IsDone())
	{
		if (generator.HasYieldValue())
		{
			int value = generator.GetYieldValue();
			std::cout << "Yield: " << value << std::endl;
		}
		generator.Resume();
	}

	if (generator.HasReturnValue())
	{
		int value = generator.GetReturnValue();
		std::cout << "Return: " << value << std::endl;
	}
	generator.Resume();
	generator.Resume();
#endif
	return 0;
}