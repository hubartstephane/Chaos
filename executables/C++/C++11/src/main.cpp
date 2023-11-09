#include <chaos/chaos.h>

class MyApplication : public chaos::Application
{
protected:

	virtual int Main() override
	{

		return 0;
	}
};

int main(int argc, char** argv, char** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}