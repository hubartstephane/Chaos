#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	class MIDICommand
	{
	public:

		/** default constructor */
		MIDICommand() = default;
		/** constructor with initialization */
		MIDICommand(uint32_t value) 
		{ 
			SetValue(value); 
		}
		/** constructor with initialization */
		MIDICommand(unsigned char in_command, unsigned char in_param1 = 0, unsigned char in_param2 = 0, unsigned char in_param3 = 0) 
		{ 
			SetValue(in_command, in_param1, in_param2, in_param3); 
		}

		/** setter */
		void SetValue(unsigned char in_command, unsigned char in_param1, unsigned char in_param2, unsigned char in_param3)
		{
			command = in_command;
			param1 = in_param1;
			param2 = in_param2;
			param3 = in_param3;

			command_id = command & ~15; // remove the channel part
			channel = command & 15;
		}
		/** setter */
		void SetValue(uint32_t value)
		{
			command = (unsigned char)((value >> 0) & 0xFF);
			param1  = (unsigned char)((value >> 8) & 0xFF);
			param2  = (unsigned char)((value >> 16) & 0xFF);
			param3  = (unsigned char)((value >> 24) & 0xFF);

			command_id = command & ~15; // remove the channel part
			channel = command & 15;
		}
		/** getter */
		uint32_t GetValue() const
		{
			return  
				(((uint32_t)command) >> 0) | 
				(((uint32_t)param1) >> 8)  | 
				(((uint32_t)param2) >> 16) | 
				(((uint32_t)param3) >> 24);
		}

		/** the command */
		unsigned char command;
		/** the parameter */
		unsigned char param1;
		/** the parameter */
		unsigned char param2;
		/** the parameter */
		unsigned char param3;

		/** command whose channel information has been removed */
		unsigned char command_id;
		/** the channel information */
		unsigned char channel;
	};


}; // namespace chaos

