import modulelib;
import modulelibdll;


int main(int argc, char** argv, char** env)
{
	ModuleLibFunc();
	ModuleLibDLLFunc1(7);

	MyClass1().f();
	MyClass2().f();
	return 0;
}


