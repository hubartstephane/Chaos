
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/Application.h>



#if 0


an
amiante
#endif

#if 0


PREFIX           MIDDLE           SUFFIX
.MOT = (CONSONNE*.VOYELLE).(CONSONNE.VOYELLE)*.(CONSONNE)*

-le suffix est toujours collÈ au token d'avant

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











class TextSplitter
{
public:

	/** type for tokens */
	struct Token
	{
		Token() = default;

		Token(int s, int e, int t) :
			start(s),
			end(e),
			type(t){}

		int start{ 0 };
		int end{ 0 };
		int type{ 0 };
	};

	/** the types for tokens */
	static int const TOKEN_TYPE_UNKNOWN = 0;
	static int const TOKEN_TYPE_CONSONANT = 1;
	static int const TOKEN_TYPE_VOWEL = 2;
	static int const TOKEN_TYPE_SPECIAL = 3;

	/** returns true whether entry is a vowel */
	static bool IsVowel(char c);
	/** returns true whether entry is a consonant */
	static bool IsConsonant(char c);

	/** cut a string into tokens */
	static std::vector<Token> Tokenize(char const * str);
	/** an utility method to have a debug string from a tokenized result */
	static std::string GetDebugCutString(char const * str, std::vector<TextSplitter::Token> const & tokens);

protected:

	/** internal method to cut a string in tokens */
	static std::vector<Token> TokenizePerType(char const * str);
	/** internal method to cut consonnant tokens if a character repetition is found */
	static void SplitDoubleConsonantTokens(char const * str, std::vector<TextSplitter::Token> & tokens);
	/** internal method to merge tokens as non splittable unities */
	static void MergeTokens(char const * str, std::vector<TextSplitter::Token> & tokens);
};


bool TextSplitter::IsVowel(char c)
{
	char const * vowel = "aeiouy‚ÍÓÙ˚‰ÎÔˆ¸ˇÈË‡";
	return (strchr(vowel, c) != nullptr);
}

bool TextSplitter::IsConsonant(char c)
{
	char const * consonant = "bcdfghjklmnpqrstvwxz";
	return (strchr(consonant, c) != nullptr);
}

std::vector<TextSplitter::Token> TextSplitter::Tokenize(char const * str)
{
	std::vector<TextSplitter::Token> result = TokenizePerType(str);
	SplitDoubleConsonantTokens(str, result);
	MergeTokens(str, result);
	return result;
}

std::string TextSplitter::GetDebugCutString(char const * str, std::vector<TextSplitter::Token> const & tokens)
{
	std::string result;
	result.reserve(strlen(str) + tokens.size());

	for (size_t i = 0; i < tokens.size(); ++i)
	{
		TextSplitter::Token const & token = tokens[i];
		for (int j = token.start; j <= token.end; ++j)
			result += str[j];
		if (i != tokens.size() - 1)
			result += "|";
	}
	return result;
}

std::vector<TextSplitter::Token> TextSplitter::TokenizePerType(char const * str)
{
	std::vector<TextSplitter::Token> result;

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
			result.emplace_back(i, j - 1, TOKEN_TYPE_VOWEL);
			i = j;
		}
		// test for consonant
		j = i;
		while (str[j] != 0 && IsConsonant(str[j]))
			++j;
		if (i != j)
		{
			result.emplace_back(i, j - 1, TOKEN_TYPE_CONSONANT);
			i = j;
		}
		// test for other special characters
		j = i;
		while (str[j] != 0 && (!IsVowel(str[j]) && !IsConsonant(str[j])))
			++j;
		if (i != j)
		{
			result.emplace_back(i, j - 1, TOKEN_TYPE_SPECIAL);
			i = j;
		}
	}
	return result;
}

void TextSplitter::SplitDoubleConsonantTokens(char const * str, std::vector<TextSplitter::Token> & tokens)
{
	std::vector<TextSplitter::Token> result;
	for (size_t i = 0; i < tokens.size(); ++i)
	{
		TextSplitter::Token & token = tokens[i];

		// ignore NON CONSONANT and ONE LETTER group
		if (token.type != TOKEN_TYPE_CONSONANT || token.start == token.end)
		{
			result.push_back(token);
			continue;
		}
		// search any CONSONNANT REPETITION
		int j = token.start;
		for (; j < token.end; ++j)
		{
			if (str[j] == str[j + 1])
			{
				result.emplace_back(token.start, j, TOKEN_TYPE_CONSONANT);
				result.emplace_back(j + 1, token.end, TOKEN_TYPE_CONSONANT);
				break;
			}
		}
		// no repetition
		if (j == token.end)
			result.push_back(token);
	}
	std::swap(tokens, result);
}

void TextSplitter::MergeTokens(char const * str, std::vector<TextSplitter::Token> & tokens)
{
	std::vector<Token> result = tokens;


	std::swap(result, tokens);
}




#if 0

void MergeTokens(char const * str, std::vector<Token> & tokens)
{
	std::vector<Token> result;


	int current_type = TOKEN_TYPE_UNKNOWN;

	int i = 0;



	for (size_t i = 0; i < tokens.size() - 1; ++i)
	{








		Token & token1 = tokens[i];
		assert(token1.type_start == token1.type_end);
		if (token1.type_start == TYPE_SPECIAL) // cannot merge nothing with Group-Special
			continue;

		Token & token2 = tokens[i + 1];
		assert(token2.type_start == token2.type_end);
		if (token2.type_start == TYPE_SPECIAL) // cannot merge nothing with Group-Special
			continue;
		
		if (token1.type_end == token2.type_start) // cannot merge groups of same type
			continue;





	}
	//std::swap(tokens, result);
}





#endif




void TestCutWord(char const * str)
{
	std::vector<TextSplitter::Token> tokens = TextSplitter::Tokenize(str);

	std::string debug_string = TextSplitter::GetDebugCutString(str, tokens);

	chaos::LogTools::Log("TestCutWord[%s] = [%s]", str, debug_string.c_str());
}



int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	boost::filesystem::path resources_path = chaos::Application::GetInstance()->GetResourcesPath();
	
	std::vector<std::string> dictionnary = chaos::FileTools::ReadFileLines((resources_path / "dictionnaire.txt").string().c_str());

	TestCutWord("bonjour");
	TestCutWord("peuple");
	TestCutWord("trottinette");
	TestCutWord("lance-pierre");
	TestCutWord("aqueuse");
	TestCutWord("autour");
	TestCutWord("ancien");



  chaos::WinTools::PressToContinue();

  chaos::Application::Finalize();

  return 0;
}


