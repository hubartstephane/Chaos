
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/Application.h>


bool IsVowel(char c)
{
	char const * vowel = "aeiouyâêîôûäëïöüÿéèà";
	return (strchr(vowel, c) != nullptr);
}

bool IsConsonant(char c)
{
	char const * consonant = "bcdfghjklmnpqrstvwxz";
	return (strchr(consonant, c) != nullptr);
}



#if 0


an
amiante
#endif

#if 0


PREFIX           MIDDLE           SUFFIX
.MOT = (CONSONNE*.VOYELLE).(CONSONNE.VOYELLE)*.(CONSONNE)*

-le suffix est toujours collé au token d'avant

plain | te plai | ne

WORD = (GROUPE CONSONNE).(GROUPE VOYELLE))

GROUPE VOYELLE = VOYELLE*
(aeiouy)*
an->si n est pas suivie d une voyelle : a | na | na | s
am ->
en
em
ain
aim

b
b o
b o n

#endif

std::vector<std::string> ReadFileLines(boost::filesystem::path const & p)
{
	std::vector<std::string> result;

	std::ifstream file(p.c_str());
	if (file)
	{
		std::copy(std::istream_iterator<std::string>(file),
							std::istream_iterator<std::string>(),
							std::back_inserter(result));
	}
	return result;
}

int const TYPE_CONSONANT = 0;
int const TYPE_VOWEL = 1;
int const TYPE_SPECIAL = 1;


struct Token
{
	Token() = default;

	Token(int s, int e, int t) : start(s), end(e), type(t) {}

	int start{ 0 };
	int end{ 0 };
	int type{ 0 };
};





std::vector<Token> CutStringPerTypes(char const * str)
{
	std::vector<Token> result;

	int i = 0;
	int j = 0;
	while (str[i] != 0)
	{
		// test for vowels
		j = i;
		while (str[j] != 0 && IsVowel(str[j]))
			++j;
		if (i != j)
		{
			result.emplace_back(i, j - 1, TYPE_VOWEL);
			i = j;
		}
		// test for consonant
		j = i;
		while (str[j] != 0 && IsConsonant(str[j]))
			++j;
		if (i != j)
		{
			result.emplace_back(i, j - 1, TYPE_CONSONANT);
			i = j;
		}
		// test for other special characters
		j = i;
		while (str[j] != 0 && (!IsVowel(str[j]) && !IsConsonant(str[j])))
			++j;
		if (i != j)
		{
			result.emplace_back(i, j - 1, TYPE_SPECIAL);
			i = j;
		}
	}
	return result;
}

std::string GetDebugCutResultString(char const * str, std::vector<Token> const & tokens)
{
	std::string result;
	result.reserve(strlen(str) + tokens.size());

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		Token const & token = tokens[i];
		for (int j = token.start; j <= token.end; ++j)
			result += str[j];
		if (i != tokens.size() - 1)
			result += "|";
	}
	return result;
}


std::vector<Token> GenerateTokens(char const * str)
{
	std::vector<Token> result = CutStringPerTypes(str);


	return result;
}

void TestCutWord(char const * str)
{
	std::vector<Token> tokens = GenerateTokens(str);

	std::string debug_string = GetDebugCutResultString(str, tokens);

	chaos::LogTools::Log("TestCutWord[%s] = [%s]", str, debug_string.c_str());
}



int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	boost::filesystem::path resources_path = chaos::Application::GetInstance()->GetResourcesPath();
	
//	std::vector<std::string> dictionnary = ReadFileLines(resources_path / "dictionnaire.txt");

	TestCutWord("bonjour");
	TestCutWord("peuple");
	TestCutWord("trottinette");
	TestCutWord("autour");
	TestCutWord("ancien");



  chaos::WinTools::PressToContinue();

  chaos::Application::Finalize();

  return 0;
}


