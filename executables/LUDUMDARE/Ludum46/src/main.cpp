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



// ============================================================================

class ExecutionContextData : public chaos::Object
{
	friend class ExecutionContext;

protected:

	std::vector<std::function<void()>> completion_function;

	std::atomic<int> pending_task;
};

class ExecutionContext
{
public:

	void AddTask()
	{
		ConditionalCreateData();
		++context_data->pending_task;
	}

	void CompleteTask()
	{
		if (!IsCompleted())
		{
			if (--context_data->pending_task == 0)
			{
				for (auto func : context_data->completion_function)
					func();
			}
		}
	}

	bool IsCompleted() const
	{
		if (context_data == nullptr)
			return true;
		if (context_data->pending_task == 0)
			return true;
		return false;
	}

	void OnCompleted(std::function<void()> func)
	{
		if (IsCompleted())
			func();
		else
			context_data->completion_function.push_back(func);
	}

protected:

	void ConditionalCreateData()
	{
		if (context_data == nullptr)
			context_data = new ExecutionContextData;
	}


protected:

	chaos::shared_ptr<ExecutionContextData> context_data;
};

// quand on detruit le future d un async, c 'est bloquant


ExecutionContext F()
{
	ExecutionContext result;

	

	for (int i = 1; i < 3; ++i)
	{
		result.AddTask();
		std::thread t([result, i]() mutable
		{
			std::this_thread::sleep_for(std::chrono::seconds(5*i));
			result.CompleteTask();
		});
		t.detach();
	}



	int i = 0;
	++i;


	return result;
}




class FFF
{
	template<typename T, typename R>
	void fff(std::function<R(T)> func)
	{

	}


};


// ============================================================================

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

	auto glambda = [](auto a, auto b) { return a + b; };

	auto fm = glambda(1.0f, 2.0f);
	auto fg = glambda(1, 2);

	[=] {};

#if 0
void (*foo)(bool, int);
foo = [argc](bool, int) 
{
	//argc = argc;


};
	FFF().fff([](int *)
	{
		return 3;
	});

#endif










	ExecutionContext context = F();
	context.OnCompleted([]() 
	{
		int i = 0;
		++i;
	
	});


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

#if 0

	FFF().fff([](int *)
	{
		return 3;
	});




	chaos::ClassLoader loader;
	loader.LoadClassesInDirectory("resources/classes");

	auto fffx = [](toto* ob)
	{
		ob = ob;
	};

	std::function<void(toto *)> fff = [](toto* ob)
	{
		ob = ob;
	};

			auto ccc = chaos::Class::FindClass("classes2");


			chaos::SubClassOf<chaos::Object> sc = ccc;

		sc.CreateInstanceOnStack([](chaos::Object * obj)
		{
			obj = obj;

		});
		
#if 0
		ccc->CreateInstanceOnStack2([](toto * ob)
		{
			ob = ob;
		});
#endif

		ccc->CreateInstanceOnStack2(fff);
#endif



	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}
