# ApplicationArguments


## Use ApplicationArguments inside your program


you may easily work with arguments from command line with the class **ApplicationArgumentManager**

In your code, you just have to declare in **global scope** any variable you want and use it inside functions.

>	CHAOS_APPLICATION_ARG(type, name) or  
>	CHAOS_APPLICATION_ARG(type, name, default_value)

for example,

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

- you only have to call **chaos::ApplicationArgumentManager::ParseArguments(...)** once.

- if you use the class **chaos::Application** this will automatically be done for you.

- the macro **CHAOS_APPLICATION_ARG(...)** creates a variable in the namespace **Arguments**.

- using the macro in global scope is important. This registers some function at application initialization. In another scope, this would not happen.

- you may register multiple time the same variable in different files, as soon as they have the same type.

- you may register variables in **.h** or **.cpp**

## Starting the application


you may then start the application as follow:
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