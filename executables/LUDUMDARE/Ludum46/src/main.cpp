#include <chaos/Chaos.h>

#include "Ludum46Game.h"

class toto : public chaos::Object, public chaos::JSONSerializable
{
public:

	CHAOS_DECLARE_OBJECT_CLASS2(toto, chaos::Object)

	virtual bool SerializeFromJSON(nlohmann::json const& json) override
	{
		chaos::JSONTools::GetAttribute(json, "myvalue", myvalue);

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

	/** constructor */
	ExecutionContextData(ExecutionContextData* in_parent_context = nullptr) :
		parent_context(in_parent_context) {}
	/** check whether the task is completed */
	bool IsCompleted() const;
	/** create a child task */
	ExecutionContextData * AddChildTask();
	/** create a sequence pending task */
	void AddChildSequenceTask(std::function<void(ExecutionContext)> func);
	/** lock the context */
	void Lock();
	/** unlock the context (pending actions started) */
	void Unlock();
	/** complete leaf task */
	void CompleteTask();
	/** called to continue processing */
	void OnCompletion();
	/** called whenever a child task is being completed */
	void OnChildTaskCompleted(ExecutionContextData* child_task);
	/** add a delegate at completion */
	void AddCompletionDelegate(std::function<void()> func);

protected:

	/** a mutex to protect for concurrency actions */
	mutable std::recursive_mutex critical_section;
	/** the parent of the task */
	ExecutionContextData* parent_context = nullptr;
	/** count the number of time the object has been locked */
	int lock_count = 0;
	/** whenever the task has been completed during its lock */
	bool completed_during_lock = false;
	/** all child pending tasks */
	int child_task_count = 0;
	/** all sequencial execution */
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
	if (context_data == nullptr)
		func();
	else
		context_data->AddCompletionDelegate(func);
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

void ExecutionContextData::AddCompletionDelegate(std::function<void()> func)
{
	std::unique_lock lock(critical_section);
	if (IsCompleted())
		func();
	else
		completion_functions.push_back(func);
}

void ExecutionContextData::OnCompletion()
{
	std::unique_lock lock(critical_section);
	if (child_task_count > 0)
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
	std::unique_lock lock(critical_section);
	++lock_count;
}

void ExecutionContextData::Unlock()
{
	std::unique_lock lock(critical_section);
	if (--lock_count == 0 && completed_during_lock)
	{
		completed_during_lock = false;
		OnCompletion();
	}
}

void ExecutionContextData::CompleteTask()
{
	std::unique_lock lock(critical_section);
	// should only be called manually on a leaf task
	assert(child_task_count == 0);
	assert(pending_execution.size() == 0);
	assert(parent_context != nullptr);

	if (lock_count > 0)
		completed_during_lock = true;
	else
		OnCompletion();
}


ExecutionContextData * ExecutionContextData::AddChildTask()
{
	std::unique_lock lock(critical_section);
	++child_task_count;
	return new ExecutionContextData(this);
}

void ExecutionContextData::AddChildSequenceTask(std::function<void(ExecutionContext)> func)
{
	std::unique_lock lock(critical_section);
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
	std::unique_lock lock(critical_section);
	if (child_task_count > 0)
		return false;
	if (pending_execution.size() > 0)
		return false;
	if (lock_count > 0)
		return false;
	return true;
}

void ExecutionContextData::OnChildTaskCompleted(ExecutionContextData* child_task)
{
	std::unique_lock lock(critical_section);
	--child_task_count;
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
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
