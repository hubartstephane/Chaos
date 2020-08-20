
#include <chaos/HelpText.h>

namespace chaos
{
	namespace HelpText
	{
		static std::vector<std::string>& GetMessageList()
		{
			static std::vector<std::string> result;
			return result;
		}

		size_t PushMessage(char const* message)
		{
			assert(message != nullptr);

			std::vector<std::string> & messages = GetMessageList();

			size_t result = messages.size();
			messages.push_back(message);
			return result;
		}

		size_t FlushMessages(std::function<void(char const*)> function)
		{		
			std::vector<std::string>& messages = GetMessageList();

			size_t result = messages.size();

			std::sort(messages.begin(), messages.end());
			for (std::string const& str : messages)
				function(str.c_str());
			messages.clear();
			return result;
		}
	};

}; // namespace chaos
