#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	namespace DebugTools
	{
		std::vector<CallStackEntry> ExtractCallStack()
		{
			std::vector<CallStackEntry> result;
			ProcessCallStack([&result](char const* symbol_name, char const* filename, int line_number)
			{
				CallStackEntry entry;
				entry.symbol_name = symbol_name;
				entry.filename = filename;
				entry.line_number = line_number;
				result.push_back(std::move(entry));
			});
			return result;
		}

		void ProcessCallStack(LightweightFunction<void(char const *, char const *, int)> func)
		{
			// Note on CaptureStackBackTrace : https://msdn.microsoft.com/fr-fr/library/windows/desktop/bb204633(v=vs.85).aspx

			HANDLE process = GetCurrentProcess();

			SymSetOptions(SYMOPT_LOAD_LINES);
			SymInitialize(process, NULL, TRUE);

			ULONG frame_to_skip = 0;
			while (true)
			{
				void* stack[64];
				memset(stack, 0, sizeof(stack));

				ULONG frame_to_capture = sizeof(stack) / sizeof(stack[0]);

				unsigned short frames = CaptureStackBackTrace(frame_to_skip, frame_to_capture, stack, NULL);
				if (frames == 0)
					return;

				frame_to_skip += frame_to_capture;

				char symbol_info_buffer[sizeof(SYMBOL_INFO) + 256 * sizeof(char)];
				memset(symbol_info_buffer, 0, sizeof(symbol_info_buffer));

				SYMBOL_INFO* symbol = (SYMBOL_INFO*)symbol_info_buffer;
				symbol->MaxNameLen = 255;
				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

				for (unsigned short i = 0; (i < frames) && (stack[i] != nullptr); i++)
				{
					SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

					if (strstr(symbol->Name, "DebugTools") != nullptr) // ignore all function in DebugTools namespace from the callstack
						continue;

					IMAGEHLP_LINE64 line;
					DWORD           dwDisplacement;
					line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

					if (SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &dwDisplacement, &line))
					{
						func(symbol->Name, line.FileName, line.LineNumber);
					}
				}
			}
		}

		// code inspired from OpenGL insights : chapter 33
		void DisplayCallStack(std::ostream& stream)
		{
			ProcessCallStack([&stream](char const* symbol_name, char const* filename, int line_number)
			{
				stream << "- " << filename << "(" << line_number << ")    : " << symbol_name << '\n';
			});
		}

	}; // namespace DebugTools

}; // namespace chaos
