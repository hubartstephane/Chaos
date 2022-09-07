# ApplicationArguments


## Use ApplicationArguments inside your program


You may easily work with arguments from command line with the class **ApplicationArgumentManager**

In your code, you just have to declare in **global scope** any variable you want and use it inside functions.

>	CHAOS_APPLICATION_ARG(type, name) or  
>	CHAOS_APPLICATION_ARG(type, name, default_value)

For example,

>	CHAOS_APPLICATION_ARG(bool, mybool)  
>	CHAOS_APPLICATION_ARG(int,  myint)  
>	CHAOS_APPLICATION_ARG(std::string,  mystring)  
>	CHAOS_APPLICATION_ARG(std::vector<int>,  myvectorint)  

this will generate some data (in namespace **Arguments**) that will help you get the values from command line.  
Then in any function you may get the values.
>
> int main(int argc, char **argv)  
> {  
> &ensp;&ensp;chaos::ApplicationArgumentManager::GetInstance()->ParseArguments(argc, argv);  
>   
> &ensp;&ensp;if(**Arguments::mybool.Get()**)  
> &ensp;&ensp;{  
> &ensp;&ensp;&ensp;&ensp;..  
> &ensp;&ensp;}  
> }  

<ins>Notes</ins>  

- You only have to call **chaos::ApplicationArgumentManager::ParseArguments(...)** once.

- If you use the class **chaos::Application** this will automatically be done for you.

- The macro **CHAOS_APPLICATION_ARG(...)** creates a variable in the namespace **Arguments**. This variable is a getter on the parameter that has been passed to the program.

- You may register multiple times the same name in different files, as soon as they have the same type. This will produce multiple getters, but they will all references the same parameter

- The system is case insensitive. You can do this  

> &ensp;&ensp;CHAOS_APPLICATION_ARG(bool, mybool)&ensp;&ensp;&ensp;&ensp;in file1.cpp  
> &ensp;&ensp;CHAOS_APPLICATION_ARG(bool, MYBOOL)&ensp;&ensp;&ensp;&ensp;in file2.cpp  

- Using the macro in global scope is important. This registers some function at application initialization. In another scope, this would not happen.

- You may register variables in **.h** or **.cpp**

## Starting the application


You may then start the application as follow:
> application.exe --mybool     (this set value to true)
> application.exe --mybool true  
> application.exe --mybool=true  
>  
> application.exe --myint=123  
> application.exe --myint 123
>  
> application.exe --mystring "word1 word2"  
> application.exe --mystring="word1 word2"  
>  
> application.exe --myvectorint 1 2 3 4  
> application.exe --myvectorint=1 2 3 4