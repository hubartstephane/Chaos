#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/BufferReader.h>

namespace chaos
{

	class MIDICommand
	{
	public:

		//
		// XXX : the CMD given by MIDI (extracted from Petzol, windows programming 5e. p 1478) 
		//
		// CMD_NOTE_OFF : 0x80 kk vv
		//    kk : key 
		//    vv : velocity
		//
		// CMD_NOTE_ON  : 0x90 kk vv
		//    kk : key 
		//    vv : velocity
		//
		// CMD_POLYPHONIC_AFTER_TOUCH : 0xA0 kk tt
		//    kk : key 
		//    tt : after touch
		//
		// CMD_CONTROL_CHANGE : 0xB0 cc xx
		//   CHANNEL MODE LOCAL CONTROL : 0xB0 7A xx    (xx = 0 for off, xx = 127 for on)
		//   ALL_NOTES OFF              : 0xB0 7B 00
		//   OMNI MODE OFF              : 0xB0 7C 00
		//   OMNI MODE ON               : 0xB0 7D 00
		//   MONO MODE ON               : 0xB0 7E cc    (cc : number of channel)
		//   POLY MODE ON               : 0xB0 7F 00
		//
		// CMD_PROGRAM_CHANGE : 0xC0 pp
		//    pp : program
		//
		// CMD_CHANNEL_AFTER_TOUCH : 0xD0 tt
		//    tt : after touch
		// 
		// CMD_PITCH_WHEEL_CHANGE : 0xE0 ll hh
		//    ll : low 7 bits
		//    hh : high 7 bits
		//
		// CMD_SYSTEM_MESSAGE : 0xF0 ?? ?? ?? ??
		//    0xF0 - 0xF7 : common messages
		//    0xF8 - 0xFF : realtime messages
		//
		// MIDI Message = CMD + DATA (0, 1 or 2 bytes)
		//
		//   CMD (or STATUS) : always has bit 7 set
		//   DATA            : never have bit 7 set
		//
		//
		//   CMD = 0xUV (except for SYSTEM MESSAGES 0xF0 - 0xFF that have no channel)
		//      U = message type
		//      V = channel concerned 
		//
		static const unsigned int CMD_NOTE_OFF = 0x80; 
		static const unsigned int CMD_NOTE_ON  = 0x90; 
		static const unsigned int CMD_POLYPHONIC_AFTER_TOUCH  = 0xA0; 
		static const unsigned int CMD_CONTROL_CHANGE = 0xB0; 
		static const unsigned int CMD_PROGRAM_CHANGE = 0xC0; 
		static const unsigned int CMD_CHANNEL_AFTER_TOUCH = 0xD0; 
		static const unsigned int CMD_PITCH_WHEEL_CHANGE  = 0xE0; 
		static const unsigned int CMD_SYSTEM_MESSAGE = 0xF0;

		/** default constructor */
		MIDICommand() = default;
		/** constructor with initialization */
		MIDICommand(uint32_t value) 
		{ 
			SetValue(value); 
		}
		/** constructor with initialization */
		MIDICommand(unsigned char in_status, unsigned char in_param1 = 0, unsigned char in_param2 = 0, unsigned char in_param3 = 0)
		{ 
			SetValue(in_status, in_param1, in_param2, in_param3);
		}

		/** setter */
		void SetValue(unsigned char in_status, unsigned char in_param1, unsigned char in_param2, unsigned char in_param3)
		{
			status = in_status;
			params[0] = in_param1;
			params[1] = in_param2;
			params[2] = in_param3;
		}
		/** setter */
		void SetValue(uint32_t value)
		{
			status = (unsigned char)((value >> 0) & 0xFF);
			params[0] = (unsigned char)((value >> 8) & 0xFF);
			params[1] = (unsigned char)((value >> 16) & 0xFF);
			params[2] = (unsigned char)((value >> 24) & 0xFF);
		}
		/** getter */
		uint32_t GetValue() const
		{
			return  
				(((uint32_t)status) << 0) |
				(((uint32_t)params[0]) << 8)  |
				(((uint32_t)params[1]) << 16) |
				(((uint32_t)params[2]) << 24);
		}
		/** returns true if the message is a system message */
		bool IsSystemMessage() const
		{
			return (status >= 0xF0 && status <= 0xFF);
		}
		/** returns true if the message is a common system message */
		bool IsSystemCommonMessage() const
		{
			return (status >= 0xF0 && status <= 0xF7);
		}
		/** returns true if the message is a realtime system message */
		bool IsSystemRealtimeMessage() const
		{
			return (status >= 0xF8 && status <= 0xFF);
		}
		/** returns true if the message is a not off message */
		bool IsNoteOffMessage() const
		{
			return (GetCommand() == CMD_NOTE_OFF);
		}
		/** returns true if the message is a note on message */
		bool IsNoteOnMessage() const
		{
			return (GetCommand() == CMD_NOTE_ON);
		}

		bool IsNoteEndMessage() const
		{
			return IsNoteOnMessage() && (params[1] == 0);
		}

		/** gets the channel of the message (lower 4 bits of status) */
		unsigned char GetChannel() const
		{
			return (status & 0x0F);
		}
		/** gets the command (status without 4 lower bits) */
		unsigned char GetCommand() const
		{
			return (status & 0xF0);
		}

		/** read the command from a buffer reader */
		bool ReadParams(BufferReader & reader);

		/** get number of bytes for the command */
		static int GetCommandParamCount(unsigned char status);

public:

		/** the command */
		unsigned char status;
		/** the parameters */
		unsigned char params[3];
	};


}; // namespace chaos

