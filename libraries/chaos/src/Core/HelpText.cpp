
#include <chaos/Chaos.h>

namespace chaos
{
	namespace HelpText
	{
		static std::map<std::string, std::vector<std::string>>& GetMessageMap()
		{
			static std::map<std::string, std::vector<std::string>> result;
			return result;
		}

		static std::vector<std::string>& GetMessageList(char const * family)
		{
			return GetMessageMap()[family];
		}

		size_t PushMessage(char const* family, char const* message)
		{
			assert(family  != nullptr);
			assert(message != nullptr);

			std::vector<std::string> & messages = GetMessageList(family);

			size_t result = messages.size();
			messages.push_back(message);
			return result;
		}

		size_t FlushMessages(std::function<void(char const*, char const *)> function)
		{	
			size_t result = 0;

			std::map<std::string, std::vector<std::string>> & message_map = GetMessageMap();

			for (auto it = message_map.begin(); it != message_map.end(); ++it)
			{
				std::string const & family = it->first;
				std::vector<std::string> & messages = it->second;

				size_t message_count = messages.size();
				if (message_count > 0)
				{
					// starting a new family (nullptr as message)
					function(family.c_str(), nullptr);

					// iterate over all messages of this family
					std::sort(messages.begin(), messages.end());
					for (std::string const& str : messages)
						function(family.c_str(), str.c_str());
					messages.clear();
					// update result
					result += message_count;
				}
			}

			message_map.clear();
			return result;
		}
	};

}; // namespace chaos
