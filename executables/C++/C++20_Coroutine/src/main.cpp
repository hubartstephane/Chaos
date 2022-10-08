#include "chaos/Chaos.h"

#include <coroutine>



template<typename RETURN_TYPE = void, typename YIELD_TYPE = void, bool START_SUSPENDED = true>
class Task;

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromise;

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskInternal;

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
concept Awaitable = requires(T t)
{
	{t.await_resume()};
	{t.await_ready()};
	{t.await_suspend(std::coroutine_handle<>{})};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED>
class TaskAwaiter
{
public:

	using task_type = Task<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED>;

	TaskAwaiter(task_type in_task) :
		task(in_task)
	{
	}

	bool await_ready()
	{
		return false;
	}

	auto await_suspend(std::coroutine_handle<> in_coroutine /* the coroutine about to be suspended */) -> std::coroutine_handle<TaskPromise<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, task_type>>
	{
		if (task.task_internal->handle != nullptr)
		{
			// check whether the coroutine wants to be abort
			if (task.task_internal->CheckAbortFunctions())
				return nullptr;
			// call awaited task if any instead of doing our own code
			if (TaskInternalBase<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, task_type> * other_task_internal = task.task_internal->GetAwaitedTask())
				return other_task_internal->handle;
			// continue our coroutine
			return task.task_internal->handle; // enter directly into the incomming Task
		}
		return nullptr;
	}

	decltype(auto) await_resume()
	{
		assert(task.IsDone());
		return task.GetReturnValue();
	}

public:

	/** the task we are waiting for */
	task_type task;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromiseBase
{
public:

	using task_internal_type = TaskInternal<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, TASK_TYPE>;

public:

	/** the pointer on the shared internal data */
	chaos::weak_ptr<task_internal_type> task_internal;
};

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromiseBaseReturn : public TaskPromiseBase<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, TASK_TYPE>
{
public:

	/** return a value */
	void return_value(RETURN_TYPE result)
	{
		if (this->task_internal != nullptr)
			this->task_internal->r_value = std::move(result);
	}
};


template<typename YIELD_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromiseBaseReturn<void, YIELD_TYPE, START_SUSPENDED, TASK_TYPE> : public TaskPromiseBase<void, YIELD_TYPE, START_SUSPENDED, TASK_TYPE>
{
public:

	/** co routine ends */
	void return_void()
	{
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromiseBaseReturnAndYield : public TaskPromiseBaseReturn<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, TASK_TYPE>
{
public:

	/** emit a value */
	auto yield_value(YIELD_TYPE value)
	{
		if (this->task_internal != nullptr)
			this->task_internal->y_value = std::move(value);
		return std::suspend_always{};
	}
};

template<typename RETURN_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromiseBaseReturnAndYield<RETURN_TYPE, void, START_SUSPENDED, TASK_TYPE> : public TaskPromiseBaseReturn<RETURN_TYPE, void, START_SUSPENDED, TASK_TYPE>
{
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskPromise : public TaskPromiseBaseReturnAndYield<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, TASK_TYPE>
{
public:

	using task_internal_type = TaskInternal<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, TASK_TYPE>;
	using task_type = TASK_TYPE;

	/** destructor */
	~TaskPromise()
	{
		if (this->task_internal != nullptr)
			this->task_internal->handle = nullptr;
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
		this->task_internal = new task_internal_type();
		if (this->task_internal != nullptr)
			this->task_internal->handle = std::coroutine_handle<TaskPromise>::from_promise(*this);
		return task_type(this->task_internal.get());
	}

	/** create an awaiter from a task */
	template<typename OTHER_RETURN_TYPE, typename OTHER_YIELD_TYPE, bool OTHER_START_SUSPENDED>
	auto await_transform(Task<OTHER_RETURN_TYPE, OTHER_YIELD_TYPE, OTHER_START_SUSPENDED> task)
	{
		if (task.task_internal != nullptr)
		{
			this->task_internal->awaited_task_internal = task.task_internal.get(); // current task is now waiting for input task
		}
		return TaskAwaiter<OTHER_RETURN_TYPE, OTHER_YIELD_TYPE, OTHER_START_SUSPENDED >(task);
	}

	/** if co_await an already Awaitable, no need to create anything */
	template<Awaitable T>
	T const & await_transform(T const& awaitable)
	{
		return awaitable;
	}
	/** if co_await an already Awaitable, no need to create anything */
	template<Awaitable T>
	T & await_transform(T & awaitable)
	{
		return awaitable;
	}
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

template<typename RETURN_TYPE>
class TaskInternalBaseReturn : public TaskInternalBase
{
public:

	/** check whether coroutine has a returned value */
	bool HasReturnValue() const
	{
		return (r_value.has_value());
	}
	/** read returned value*/
	RETURN_TYPE GetReturnValue() const
	{
		assert(HasReturnValue());
		return *r_value;
	}

public:

	/** the return value */
	std::optional<RETURN_TYPE> r_value;
};

template<>
class TaskInternalBaseReturn<void> : public TaskInternalBase
{
public:

	/** check whether coroutine has a returned value */
	bool HasReturnValue() const
	{
		return false;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename RETURN_TYPE, typename YIELD_TYPE>
class TaskInternalBaseReturnAndYield : public TaskInternalBaseReturn<RETURN_TYPE>
{
public:

	/** check whether coroutine has a yielded value */
	bool HasYieldValue() const
	{
		return (y_value.has_value());
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

public:

	/** the last yielded value */
	std::optional<YIELD_TYPE> y_value;
};


template<typename RETURN_TYPE>
class TaskInternalBaseReturnAndYield<RETURN_TYPE, void> : public TaskInternalBaseReturn<RETURN_TYPE>
{
public:

	bool HasYieldValue() const
	{
		return false;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED, typename TASK_TYPE>
class TaskInternal : public TaskInternalBaseReturnAndYield<RETURN_TYPE, YIELD_TYPE>
{
public:

	using task_promise_type = TaskPromise<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, TASK_TYPE>;
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
			if (CheckAbortFunctions())
				return;
			// call awaited task if any instead of doing our own code
			if (TaskInternalBase* other_task_internal = GetAwaitedTask())
			{
				other_task_internal->Resume();
				return;
			}
			// continue our coroutine
			handle.resume();
		}
	}

	/** add a function to check whether the current coroutine is to be abort */
	void AddAbortFunction(std::function<bool()> func)
	{
		abort_functions.push_back(std::move(func));
	}

	/** check whether the coroutine should be abort (and destroy handle if necessary) */
	bool CheckAbortFunctions()
	{
		for (auto const& func : abort_functions)
		{
			if (func())
			{
				handle.destroy();
				return true;
			}
		}
		return false;
	}

	/** returns whether the awaited task should be started */
	TaskInternalBase * GetAwaitedTask()
	{
		if (awaited_task_internal != nullptr)
		{
			if (awaited_task_internal->IsDone())
			{
				awaited_task_internal = nullptr;
			}
			else
			{
				return awaited_task_internal.get();
			}
		}
		return nullptr;
	}

public:

	/** function called each time a task is resumed to know whether the function has been interrupted */
	std::vector<std::function<bool()>> abort_functions;
	/** a task that is to be woken-up before we are resumed */
	chaos::shared_ptr<TaskInternalBase> awaited_task_internal;
	/** the handle for the coroutine */
	promise_handle handle;
};



/////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename RETURN_TYPE, typename YIELD_TYPE, bool START_SUSPENDED>
class Task
{
public:

	using task_internal_type = TaskInternal<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, Task>;
	using promise_type = TaskPromise<RETURN_TYPE, YIELD_TYPE, START_SUSPENDED, Task>;

	template<typename OTHER_RETURN_TYPE, typename OTHER_YIELD_TYPE, bool OTHER_START_SUSPENDED>
	friend class TaskAwaiter;

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
		return (task_internal == nullptr) || (task_internal->IsDone());
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



/////////////////////////////////////////////////////////////////////////////////////////////////////

Task<int, void> Generator2(int i)
{
	std::cout << "Generator2(" << i << ")" << std::endl;
	co_return 6 * i;
}



Task<int, void> Generator1()
{
	int result = 0;
	std::cout << "Generator1: step 1" << std::endl;
	result += co_await Generator2(1);
	std::cout << "Generator1: step 2" << std::endl;
	result += co_await Generator2(2);
	std::cout << "Generator1: step 3" << std::endl;
	result += co_await Generator2(3);
	std::cout << "Generator1: final step" << std::endl;
	co_return result;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////

Task<int, void> Generator3()
{
	while (true)
	{
		std::cout << "Generator3" << std::endl;
		co_await std::suspend_always{};
	}
	co_return 1;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv, char** env)
{
	// TEST 1 ------------------------------------------------------------------------
	if (1)
	{
		Task<int, void> G = Generator1();

		while (!G.IsDone())
		{
			std::cout << "MAIN" << std::endl;
			G.Resume();
		}
	}
	// TEST 2 ------------------------------------------------------------------------
	if (1)
	{
		auto start_time = std::chrono::system_clock::now();

		Task<int, void> G = Generator3().CancelIf([start_time]()
		{
			auto t = std::chrono::system_clock::now();
			std::chrono::duration<float> diff = t - start_time;
			
			return (diff.count() > 2.0f);
		});

		while (!G.IsDone())
		{
			std::cout << "MAIN" << std::endl;
			G.Resume();
		}
	}


	return 0;
}