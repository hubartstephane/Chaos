#include <chaos/Chaos.h>

#include "Ludum46Game.h"

class toto : public chaos::Object, public chaos::JSONSerializable
{
public:

	CHAOS_DECLARE_OBJECT_CLASS2(toto, chaos::Object)

	virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override
	{
		chaos::JSONTools::GetAttribute(json_entry, "myvalue", myvalue);

		return true;
	}

	int myvalue = 12345;
};

#if 0


	ExecutionContext BuildChildSequence(std::function<void(ExecutionContext context_sequence)> build_func)
	{
		Lock();
		ExecutionContext result = AddChildSequence();
		result.Lock();
		build_func(result);
		result.Unlock();
		Unlock();
		return result;
	}



	// prevent the dispatch of completion event
	void Lock()
	{
		ConditionalCreateData();
		++context_data->lock_count;

	}

	void Unlock()
	{
		ConditionalCreateData();
		if (--context_data->lock_count == 0)
		{

		}
	}


#endif

// ============================================================================

class ExecutionContextData;

class ExecutionContext
{
	friend class ExecutionContextData;

protected:

	/** constructor with data */
	ExecutionContext(ExecutionContextData* in_context_data): 
		context_data(in_context_data){}

public:

	/** default constructor */
	ExecutionContext() = default;
	/** copy constructor */
	ExecutionContext(ExecutionContext const &) = default;
	/** default constructor */
	ExecutionContext(ExecutionContext &&) = default;


	/** set the task to be completed */
	void CompleteTask();
	/** create a child task */
	ExecutionContext AddChildTask();
	/** create a child sequence task */
	void AddChildSequenceTask(std::function<void(ExecutionContext)> func);

	/** add (or call directly) a delegate whenever the task is being finished */
	void AddCompletionDelegate(std::function<void()> func);

	/** returns whether the task is already completed */
	bool IsCompleted() const;
	/** returns whether the task is already completed */
	operator bool() const;

	/** prevent the dispatch of completion event */
	void Lock();
	/** enable the dispatch of completion event */
	void Unlock();

protected:

	/** the context data */
	chaos::shared_ptr<ExecutionContextData> context_data;
};



// ======================================================

class ExecutionContextData : public chaos::Object
{
	friend class ExecutionContext;

protected:

	ExecutionContextData(ExecutionContextData* in_parent_context = nullptr) :
		parent_context(in_parent_context) {}

	virtual void OnChildTaskCompleted(ExecutionContextData* child_task);

	bool IsCompleted() const;

	ExecutionContextData * AddChildTask();

	void AddChildSequenceTask(std::function<void(ExecutionContext)> func);

	void Lock();
	void Unlock();

	void CompleteTask();

	void OnCompletion();
	
protected:

	/** the parent of the task */
	ExecutionContextData* parent_context = nullptr;

	/** count the number of time the object has been locked */
	int lock_count = 0;
	/** whenever the task has been completed during its lock */
	bool completed_during_lock = false;
	
	/** all child pending tasks */
	std::vector<ExecutionContextData *> child_tasks;

	std::vector < std::function<void(ExecutionContext)>> pending_execution;

	/** delegates to call whenever the task is completed */
	std::vector<std::function<void()>> completion_functions;
};

// ======================================================

ExecutionContext ExecutionContext::AddChildTask()
{
	if (context_data == nullptr)
		context_data = new ExecutionContextData;
	return ExecutionContext(context_data->AddChildTask());
}

void ExecutionContext::AddChildSequenceTask(std::function<void(ExecutionContext)> func)
{
	if (context_data == nullptr)
		context_data = new ExecutionContextData;
	context_data->AddChildSequenceTask(func);
}

void ExecutionContext::AddCompletionDelegate(std::function<void()> func)
{
	if (IsCompleted())
		func();
	else
		context_data->completion_functions.push_back(func);
}

void ExecutionContext::CompleteTask()
{
	if (context_data != nullptr)
		context_data->CompleteTask();
}

bool ExecutionContext::IsCompleted() const
{
	if (context_data == nullptr)
		return true;
	return context_data->IsCompleted();
}

ExecutionContext::operator bool() const
{
	return IsCompleted();
}

void ExecutionContext::Lock()
{
	if (context_data == nullptr)
		context_data = new ExecutionContextData;
	context_data->Lock();
}

void ExecutionContext::Unlock()
{
	assert(context_data != nullptr);
	context_data->Unlock();
}

// ======================================================

void ExecutionContextData::OnCompletion()
{
	if (child_tasks.size() > 0)
		return;
	// start new sequence execution
	if (pending_execution.size() > 0)
	{
		auto next_execution = *pending_execution.begin();
		pending_execution.erase(pending_execution.begin());

		ExecutionContext other_context = ExecutionContext(AddChildTask());
		next_execution(other_context);
		return;
	}
	// or really complete
	else
	{
		// the completion delegates
		for (auto& f : completion_functions)
			f();
		completion_functions.clear();
		// notify parent for the end
		if (parent_context != nullptr)
			parent_context->OnChildTaskCompleted(this);
	}
}

void ExecutionContextData::Lock()
{
	++lock_count;
}

void ExecutionContextData::Unlock()
{
	if (--lock_count == 0 && completed_during_lock)
	{
		completed_during_lock = false;
		OnCompletion();
	}
}

void ExecutionContextData::CompleteTask()
{
	// should only be called manually on a leaf task
	assert(child_tasks.size() == 0);
	assert(pending_execution.size() == 0);
	assert(parent_context != nullptr);

	if (lock_count > 0)
		completed_during_lock = true;
	else
		OnCompletion();
}


ExecutionContextData * ExecutionContextData::AddChildTask()
{
	ExecutionContextData * result = new ExecutionContextData(this);
	child_tasks.push_back(result);
	return result;
}

void ExecutionContextData::AddChildSequenceTask(std::function<void(ExecutionContext)> func)
{
	if (IsCompleted()) // include lock
	{
		ExecutionContext other_context = ExecutionContext(AddChildTask());
		func(other_context);
	}
	else
	{
		pending_execution.push_back(func);
	}
}

bool ExecutionContextData::IsCompleted() const
{
	if (child_tasks.size() > 0)
		return false;
	if (pending_execution.size() > 0)
		return false;
	if (lock_count > 0)
		return false;
	return true;
}

void ExecutionContextData::OnChildTaskCompleted(ExecutionContextData* child_task)
{
	child_tasks.erase(std::find(child_tasks.begin(), child_tasks.end(), child_task));
	if (lock_count > 0)
		completed_during_lock = true;
	else
		OnCompletion();
}












#if 1


// quand on detruit le future d un async, c 'est bloquant


ExecutionContext TestSimple1()
{
	ExecutionContext result;
	
	for (int i = 0; i < 2; ++i)
	{
		ExecutionContext child = result.AddChildTask();
		std::thread t([child, i]() mutable
		{
			std::this_thread::sleep_for(std::chrono::seconds(3 * i + 3));
			child.CompleteTask();
		});
		t.detach(); // because thread is on stack and assert
	}
	return result;
}





#endif



// ============================================================================

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	ExecutionContext task1;

	task1.AddCompletionDelegate([]()
	{
		int i = 0;
		++i;
	});



	ExecutionContext root_task = TestSimple1();

	root_task.AddCompletionDelegate([]()
	{
		int i = 0;
		++i;
	});

	root_task.AddChildSequenceTask([](ExecutionContext context) 
	{
		std::thread t([context]() mutable
		{
			std::this_thread::sleep_for(std::chrono::seconds(2));
			context.CompleteTask();
		});
		t.detach(); // because thread is on stack and assert
	});

	root_task.AddChildSequenceTask([](ExecutionContext context) 
	{
		context.CompleteTask();
	});

	root_task.AddChildSequenceTask([](ExecutionContext context) 
	{
		std::thread t([context]() mutable
		{
			std::this_thread::sleep_for(std::chrono::seconds(2));
			context.CompleteTask();
		});
		t.detach(); // because thread is on stack and assert
	});

	root_task.Lock();

	std::this_thread::sleep_for(std::chrono::seconds(10));

	root_task.Unlock();

		int i = 0;
		++i;

	while (true);







#if 0

	// ==============================================

	root_task.BuildSequence([](ExecutionContext sequence_task)
	{
		ExecutionContext B = sequence_task.AddTask([](ExecutionContext B)
		{
			...
			B.Complete();
		});

		ExecutionContext C = sequence_task.AddTask([](ExecutionContext C)
		{
			...
			C.Complete();
		});



	});

	// ==============================================

	ExecutionSequenceContext sequence_task = root_task.CreateSequenceTask();

	sequence_task.Lock(); // pour eviter si B se termine immediatement que sequence_task et root_task se termine avant la construction de C

	ExecutionContext B = sequence_task.AddTask([](ExecutionContext B)
	{
		...
		B.Complete();
	});

	ExecutionContext C = sequence_task.AddTask([](ExecutionContext C)
	{
		...
		C.Complete();
	});

	sequence_task.Unlock(); // 




	ExecutionContext context = F();
	context.OnCompleted([]() 
	{
		int i = 0;
		++i;
	
	});

	// ==============================================



#endif



#if 0

	std::promise<int> promise;

	std::thread t([]()
	{
		int i = 0;
		++i;

	});

	t.join();

	auto ff = std::async(std::launch::async, [&promise]()
	{
		auto t = std::this_thread::get_id();


		std::this_thread::sleep_for(std::chrono::seconds(10));
		promise.set_value(1234);	
	
	});


	auto t2 = std::this_thread::get_id();

	std::this_thread::sleep_for(std::chrono::seconds(3));
	ff.get();



	auto fprom = promise.get_future();
	auto iii = fprom.get();

	std::this_thread::sleep_for(std::chrono::seconds(3));

	



	std::future<float> f = std::async([]()
	{
		throw (new std::exception("toto was here"));
		std::this_thread::sleep_for(std::chrono::seconds(10));
		return 6.0f;
	});

	int i = 0;
	try
	{
		f.get();
		++i;

	}
	catch (std::exception const * e)
	{
		
		++i;


	}

	std::shared_future<float> sf = f.share();



	bool b1 = f.valid();

	std::future_status st = f.wait_for(std::chrono::seconds(0));
	f.wait();

	
	bool b2 = f.valid();
	//f.get();
#endif



	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
