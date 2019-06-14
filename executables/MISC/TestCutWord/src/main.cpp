
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/Application.h>



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

		Token(int s, int e, int ts, int te) :
			start(s),
			end(e),
			type_start(ts),
			type_end(te){}

		int start = 0;
		int end = 0;
		int type_start = 0;
		int type_end = 0;
	};

	/** the types for tokens */
	static int const TOKEN_TYPE_UNKNOWN   = 0;
	static int const TOKEN_TYPE_CONSONANT = 1;
	static int const TOKEN_TYPE_VOWEL     = 2;
	static int const TOKEN_TYPE_SPECIAL   = 3;

	/** returns true whether entry is a vowel */
	static bool IsVowel(char c);
	/** returns true whether entry is a consonant */
	static bool IsConsonant(char c);

	/** cut a string into tokens */
	static std::vector<Token> Tokenize(char const * str);
	/** an utility method to have a debug string from a tokenized result */
	static std::string GetDebugCutString(char const * str, std::vector<Token> const & tokens);

protected:

	/** internal method to cut a string in tokens */
	static std::vector<Token> TokenizePerType(char const * str);
	/** internal method to cut consonnant tokens if a character repetition is found */
	static void SplitDoubleConsonantTokens(char const * str, std::vector<Token> & tokens);
	/** internal method to merge tokens as non splittable unities */
	static void CreateTokenUnit(char const * str, std::vector<Token> & tokens);

	static Token MergeTokens(Token const & src1, Token const & src2);
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
	CreateTokenUnit(str, result);
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
			result.emplace_back(i, j - 1, TOKEN_TYPE_VOWEL, TOKEN_TYPE_VOWEL);
			i = j;
		}
		// test for consonant
		j = i;
		while (str[j] != 0 && IsConsonant(str[j]))
			++j;
		if (i != j)
		{
			result.emplace_back(i, j - 1, TOKEN_TYPE_CONSONANT, TOKEN_TYPE_CONSONANT);
			i = j;
		}
		// test for other special characters
		j = i;
		while (str[j] != 0 && (!IsVowel(str[j]) && !IsConsonant(str[j])))
			++j;
		if (i != j)
		{
			result.emplace_back(i, j - 1, TOKEN_TYPE_SPECIAL, TOKEN_TYPE_SPECIAL);
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
		assert(token.type_start == token.type_end);

		// ignore NON CONSONANT and ONE LETTER group
		if (token.type_start != TOKEN_TYPE_CONSONANT || token.start == token.end)
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
				result.emplace_back(token.start, j, TOKEN_TYPE_CONSONANT, TOKEN_TYPE_CONSONANT);
				result.emplace_back(j + 1, token.end, TOKEN_TYPE_CONSONANT, TOKEN_TYPE_CONSONANT);
				break;
			}
		}
		// no repetition
		if (j == token.end)
			result.push_back(token);
	}
	std::swap(tokens, result);
}

TextSplitter::Token TextSplitter::MergeTokens(TextSplitter::Token const & src1, TextSplitter::Token const & src2)
{
	return TextSplitter::Token(src1.start, src2.end, src1.type_start, src2.type_end);
}

void TextSplitter::CreateTokenUnit(char const * str, std::vector<TextSplitter::Token> & tokens)
{
	std::vector<Token> result;

	size_t i = 0;

	while (i < tokens.size())
	{
		// take current token and push it into result
		TextSplitter::Token & token = tokens[i++];
		assert(token.type_start == token.type_end);
		result.push_back(token); 

		if (token.type_start == TOKEN_TYPE_SPECIAL)
			continue;

		TextSplitter::Token & current_token = result[result.size() - 1];
		while (i < tokens.size())
		{
			TextSplitter::Token & incomming_token = tokens[i];
			if (incomming_token.type_start == TOKEN_TYPE_SPECIAL)
				break;

			// can merge CONSONANT + VOWEL
			if (current_token.type_end == TOKEN_TYPE_CONSONANT && incomming_token.type_start == TOKEN_TYPE_VOWEL)
			{
				current_token = MergeTokens(current_token, incomming_token);
				++i;
				continue;
			}

			// can merge VOWEL + ...
			if (current_token.type_end == TOKEN_TYPE_VOWEL && incomming_token.type_start == TOKEN_TYPE_CONSONANT)
			{
				// + ... (N | M)
				if (incomming_token.end - incomming_token.start != 0) // not single character consonant groupe
				{
					char c = str[incomming_token.start]; 
					if (c == 'n' || c == 'm' || c == 'r')
					{
						current_token.type_end = TOKEN_TYPE_VOWEL; // !!! we considere some token as 'an' as a vowel
						++current_token.end;
						++incomming_token.start; // eat the first character from one token to the other
						break;
					}
				}
				// + ... repetition consonant .. or consonnant+separator (ex : aigreS-douce)
				if (i + 1 <= tokens.size() - 1)
				{
					if (tokens[i + 1].type_start == TOKEN_TYPE_CONSONANT || tokens[i + 1].type_start == TOKEN_TYPE_SPECIAL)
					{
						current_token = MergeTokens(current_token, incomming_token);
						++i;
						break;
					}
				}					
			}
			break;
		}
	}

	// if word starts by a single vowel, merge with next => example a|bsent (a better separation would be ab|sent)
	size_t count = result.size();
#if 0
	if (count >= 2)
	{
		Token & t1 = result[0];
		Token & t2 = result[1];		
		if (t1.type_start == TOKEN_TYPE_VOWEL && t1.type_end == TOKEN_TYPE_VOWEL && t2.type_start == TOKEN_TYPE_CONSONANT)
		{
			t2 = MergeTokens(t1, t2);
			result.erase(result.begin());			
		}		
	}
#endif
	// if word finishes by a single consonnant, merge it with previous
	count = result.size();
	if (count >= 2)
	{
		Token & t1 = result[count - 2];
		Token & t2 = result[count - 1];		
		if (t1.type_end != TOKEN_TYPE_SPECIAL && t2.type_start == TOKEN_TYPE_CONSONANT && t2.type_end == TOKEN_TYPE_CONSONANT)
		{
			t1 = MergeTokens(t1, t2);
			result.pop_back();
		}		
	}


	std::swap(result, tokens);
}


void TestCutWord(char const * str)
{
	std::vector<TextSplitter::Token> tokens = TextSplitter::Tokenize(str);

	std::string debug_string = TextSplitter::GetDebugCutString(str, tokens);

	chaos::LogTools::Log("TestCutWord[%s] = [%s]", str, debug_string.c_str());
}

void CutDirectionnaryWords(char const * filename)
{
	boost::filesystem::path resources_path = chaos::Application::GetInstance()->GetResourcesPath();
	std::vector<std::string> dictionnary = chaos::FileTools::ReadFileLines(resources_path / filename);

	if (dictionnary.size() > 0)
	{
		// transform strings into the split information
		std::vector < TextSplitter::Token> tokens;
		for (std::string & str : dictionnary)
		{
			tokens = TextSplitter::Tokenize(str.c_str());			
			str = TextSplitter::GetDebugCutString(str.c_str(), tokens);
			tokens.clear();
		}

		// save the new 'directory' into a file
		boost::filesystem::path dst_p;
		if (chaos::FileTools::CreateTemporaryDirectory("TestCutWords", dst_p))
		{
			boost::filesystem::path dst_file = dst_p / "cut_words.txt";
			chaos::FileTools::WriteFileLines(dst_file, dictionnary);
			chaos::WinTools::ShowFile(dst_file);
		}
	}
}

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		TestCutWord("bonjour");
		TestCutWord("peuple");
		TestCutWord("trottinette");
		TestCutWord("lance-pierre");
		TestCutWord("aqueuse");
		TestCutWord("autour");
		TestCutWord("absent");
		TestCutWord("ancien");
		TestCutWord("comptable");
		TestCutWord("aigres-douces");


		CutDirectionnaryWords("french_dictionnary.txt");
		CutDirectionnaryWords("english_dictionnary.txt");

		chaos::WinTools::PressToContinue();

		return true;
	}
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


