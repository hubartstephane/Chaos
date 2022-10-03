#include "chaos/Chaos.h"

#include <coroutine>



template<typename YIELD_TYPE = void, typename RETURN_TYPE = void, bool START_SUSPENDED = true>
class Task;

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromise;

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskInternal;


/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
concept is_awaiter = requires(T t)
{
	{t.await_resume()};
	{t.await_ready()};
	{t.await_suspend(std::coroutine_handle<>{})};
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
	void return_value(RETURN_TYPE result)
		requires (!std::is_same_v<RETURN_TYPE, void>)
	{
		if (task_internal != nullptr)
			task_internal->r_value = std::move(result);
	}
	/** emit a value */
	auto yield_value(YIELD_TYPE value)
		requires !is_same_v<void, YIELD_TYPE>
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













	/////////////////////////////////////////////////////////////////////////

	class Awaiter
	{
	public:

		//Awaiter(Task* in_task) :
		//	task(in_task)
		Awaiter()
		{
			int i = 0;
			++i;
		}

		bool await_ready()
		{
			return false;
		}

		auto await_suspend(std::coroutine_handle<> in_p /* the coroutine about to be suspended */)
		{
			//while (!task->IsDone())
			//	task->Resume();

			//return task->task_internal->handle;

			int i = 0;
			++i;

			//p = in_p;

			//return other_task.handle; // enter other Task
		}

		auto await_resume()
		{
			//	if (!task->IsDone())
			//		task->Resume();
			return 777; // other_task.GetYieldValue();
		}

		//Task* task = nullptr;
	};

	template<typename OTHER_YIELD_TYPE, typename OTHER_RETURN_TYPE, bool OTHER_START_SUSPENDED>
	auto await_transform(Task<OTHER_YIELD_TYPE, OTHER_RETURN_TYPE, OTHER_START_SUSPENDED> task)
	{
		if (task.task_internal != nullptr)
		{
			task_internal->awaited_task_internal = task.task_internal.get();
		}
		return Awaiter();
	}

public:

	/** the pointer on the shared internal data */
	chaos::weak_ptr<task_internal_type> task_internal;
};













/////////////////////////////////////////////////////////////////////////////////////////////////////
class TaskInternalBase : public chaos::Object
{
public:

	/** check whether the task is done */
	virtual bool IsDone() const = 0;
	/** resume the task */
	virtual void Resume() = 0;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename YIELD_TYPE, typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskInternal : public TaskInternalBase
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

	/** check whether coroutine is completed */
	bool IsDone() const
	{
		return (handle == nullptr) || (handle.done());
	}

	/** resume the coroutine */
	void Resume()
	{
		if (handle != nullptr)
		{
			// check whether the coroutine wants to be abort
			for (auto const& func : abort_functions)
			{
				if (func())
				{
					handle.destroy();
					return;
				}
			}
			// call awaited task if any instead of doing our own code
			if (awaited_task_internal != nullptr)
			{
				if (awaited_task_internal->IsDone())
				{
					awaited_task_internal = nullptr;
				}
				else
				{
					awaited_task_internal->Resume();
					return;
				}
			}
			// continue our coroutine
			handle.resume();
		}
	}

	/** check whether coroutine has a yielded value */
	bool HasYieldValue() const
		requires !is_same_v<void, YIELD_TYPE>
	{
		return (y_value.has_value());
	}

	/** check whether coroutine has a returned value */
	bool HasReturnValue() const
		requires (!std::is_same_v<RETURN_TYPE, void>)
	{
		return (r_value.has_value());
	}
	/** read yielded value + destruction */
	YIELD_TYPE ConsumeYieldValue() const
		requires !is_same_v<void, YIELD_TYPE>
	{
		assert(HasYieldValue());
		YIELD_TYPE result = std::move(*y_value);
		y_value.reset();
		return result;
	}

	/** read yielded value without destroying it */
	YIELD_TYPE GetYieldValue() const
		requires !is_same_v<void, YIELD_TYPE>
	{
		assert(HasYieldValue());
		return *y_value;
	}

	/** read returned value*/
	RETURN_TYPE GetReturnValue() const
		requires (!std::is_same_v<RETURN_TYPE, void>)
	{
		assert(HasReturnValue());
		return *r_value;
	}
	/** add a function to check whether the current coroutine is to be abort */
	void AddAbortFunction(std::function<bool()> func)
	{
		abort_functions.push_back(std::move(func));
	}

public:

	/** function called each time a task is resumed to know whether the function has been interrupted */
	std::vector<std::function<bool()>> abort_functions;
	/** a task that is to be woken-up before we are resumed */
	chaos::shared_ptr<TaskInternalBase> awaited_task_internal;

	/** the last yielded value */
	std::optional<YIELD_TYPE> y_value;
	/** the return value */
	std::optional<RETURN_TYPE> r_value;
	/** the handle for the coroutine */
	promise_handle handle;
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
		requires !is_same_v<void, YIELD_TYPE>
	{
		return (task_internal != nullptr) && (task_internal->HasYieldValue());
	}
	/** check whether coroutine has a returned value */
	bool HasReturnValue() const
		requires (!std::is_same_v<RETURN_TYPE, void>)
	{
		return (task_internal != nullptr) && (task_internal->HasReturnValue());
	}
	/** read yielded value + destruction */
	YIELD_TYPE ConsumeYieldValue() const
		requires !is_same_v<void, YIELD_TYPE>
	{
		assert(task_internal != nullptr);
		return task_internal->ConsumeYieldValue();
	}
	/** read yielded value without destroying it */
	YIELD_TYPE GetYieldValue() const
		requires !is_same_v<void, YIELD_TYPE>
	{
		assert(task_internal != nullptr);
		return task_internal->GetYieldValue();
	}
	/** read returned value*/
	RETURN_TYPE GetReturnValue() const
		requires (!std::is_same_v<RETURN_TYPE, void>)
	{
		assert(task_internal != nullptr);
		return task_internal->GetReturnValue();
	}
	/** add a cancellation function */
	Task& CancelIf(std::function<bool()> func)
	{
		task_internal->AddAbortFunction(std::move(func));
		return *this;
	}

protected:

	/** the pointer to the internal data */
	chaos::shared_ptr<task_internal_type> task_internal;
};


static int counter = 0;


Task<int, int> AnotherGenerator(char A)
{
	std::cout << "AnotherGenerator" << A << std::endl;
	++counter;
	co_return 6;
}

Task<int, int> Generator(char A)
{
	std::cout << "Generator 1" << A << std::endl;
	co_await AnotherGenerator('X');
	//co_yield 1;
	std::cout << "Generator 2" << A << std::endl;
	co_await AnotherGenerator('Y');
	//co_yield 2;
	std::cout << "Generator 3" << A << std::endl;
	co_await AnotherGenerator('Z');
	co_return 5;
}

#if 0
Task<int, int> WaitFor(float seconds)
{
	auto start_time = std::chrono::system_clock::now();

	while (true)
	{
		auto t = std::chrono::system_clock::now();

		std::chrono::duration<float> diff = t - start_time;
		if (diff > seconds)
			co_return 0;
	}
}
#endif

Task<int, int> OtherGenerator()
{

	//co_await std::suspend_never;

	std::cout << "OtherGenerator 1" << std::endl;
	co_await Generator('A').CancelIf([]()
	{
		return counter == 2;
	});


	//co_await std::suspend_always{};

	std::cout << "OtherGenerator 2" << std::endl;
	co_await Generator('B');
	std::cout << "OtherGenerator 3" << std::endl;
	co_return 666;
}



template<typename RET_TYPE>
Task<RET_TYPE, RET_TYPE> WaitForAnyReturn(std::vector<Task<RET_TYPE, RET_TYPE>>& tasks)
{
	while (true)
	{
		for (auto& task : tasks)
		{
			co_await task;
			if (task.HasReturnValue())
				co_return task.GetReturnValue();
		}
	}
}

template<typename RET_TYPE>
Task<RET_TYPE, RET_TYPE> WaitForAnyYield(std::vector<Task<RET_TYPE, RET_TYPE>>& tasks)
{


	while (true)
	{
		for (auto& task : tasks)
		{
			co_await task;
			if (task.HasReturnValue())
				co_return task.GetReturnValue();
			if (task.HasYieldValue())
				co_yield task.ConsumeYieldValue();
		}
	}
}


int main(int argc, char** argv, char** env)
{
	bool b1 = is_awaiter<int>;
	bool b2 = is_awaiter<std::suspend_always>;
	bool b3 = is_awaiter<std::suspend_never>;
	bool b4 = is_awaiter<Task<int, int>>;




	auto start_time = std::chrono::system_clock::now();

	while (true)
	{
		auto t = std::chrono::system_clock::now();

		std::chrono::duration<float> diff = t - start_time;
		if (diff.count() > 7.0f)
			break;
	}











	{
		Task<int, int> generator = OtherGenerator();
		while (!generator.IsDone())
		{
			std::cout << "main:" << std::endl;
			generator.Resume();
		}

	}

#if 0
	{
		Task<int, int> generator = Generator();

		env = env;
	}
#endif

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