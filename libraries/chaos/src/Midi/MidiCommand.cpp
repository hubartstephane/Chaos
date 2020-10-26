#include <chaos/Chaos.h>

namespace chaos
{
	int MIDICommand::GetCommandParamCount(unsigned char status)
	{
		// remove channel part from the command
		status &= 0xF0; 

		if (status == CMD_NOTE_OFF)
			return 2;
		if (status == CMD_NOTE_ON)
			return 2;
		if (status == CMD_POLYPHONIC_AFTER_TOUCH)
			return 2;
		if (status == CMD_CONTROL_CHANGE)
			return 2;
		if (status == CMD_PROGRAM_CHANGE)
			return 1;
		if (status == CMD_PITCH_WHEEL_CHANGE)
			return 2;
		return -1;	
	}

	bool MIDICommand::ReadParams(BufferReader & reader)
	{
		int count = GetCommandParamCount(status);
		if (count < 0 || count > 3)
			return false;
		if (!reader.IsEnoughData((size_t)count))
			return false;
		for (int i = 0 ; i < count ; ++i)
			reader.Read(params[i]);
		return true;
	}

}; // namespace chaos
