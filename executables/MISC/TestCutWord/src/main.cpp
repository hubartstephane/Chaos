
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>


bool IsVowel(char c)
{
	char const * vowel = "aeiouy";
	return (strchr(vowel, c) != nullptr);
}

bool IsConsonant(char c)
{
	char const * consonant = "bcdfghjqlmnpqrstvwxz";
	return (strchr(consonant, c) != nullptr);
}

void TestCutWord(char const * str)
{
	
	
	
	
	for (int i = 0; str[i] != 0; ++i)
	{
		bool is_vowel = IsVowel(str[i]);
		if (is_vowel)
		{
			int j = i + 1;
			while ((str[j] != 0) && IsVowel(str[j]))
				++j;
		}

		bool is_consonant = IsConsonant(str[i]);
		if (is_consonant)
		{
			int j = i + 1;
			while ((str[j] != 0) && IsConsonant(str[j]) && (str[j] != str))
				++j;
		}

		


	}


}


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	TestCutWord("bonjour");
	TestCutWord("peuple");
	TestCutWord("trottinette");
	TestCutWord("autour");
	TestCutWord("ancien !!!");

  chaos::WinTools::PressToContinue();

  chaos::Application::Finalize();

  return 0;
}


