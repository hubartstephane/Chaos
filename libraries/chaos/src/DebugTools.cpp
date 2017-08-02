#include <chaos/DebugTools.h>

namespace chaos
{

	// code inspired from OpenGL insights : chapter 33
	void DebugTools::DisplayCallStack(std::ostream & stream)
	{
		// Note on CaptureStackBackTrace : https://msdn.microsoft.com/fr-fr/library/windows/desktop/bb204633(v=vs.85).aspx

		HANDLE process = GetCurrentProcess();

		SymSetOptions(SYMOPT_LOAD_LINES);
		SymInitialize(process, NULL, TRUE);

		ULONG frame_to_skip = 0;
		while (true)
		{
			void * stack[ 64 ];
			memset(stack, 0, sizeof(stack));

			ULONG frame_to_capture = sizeof(stack) / sizeof(stack[0]);

			unsigned short frames = CaptureStackBackTrace(frame_to_skip, frame_to_capture, stack, NULL);
			if (frames == 0)
				return;

			frame_to_skip += frame_to_capture;

			char symbol_info_buffer[sizeof(SYMBOL_INFO) + 256 * sizeof( char )];
			memset(symbol_info_buffer, 0, sizeof(symbol_info_buffer));

			SYMBOL_INFO  * symbol = (SYMBOL_INFO *)symbol_info_buffer;
			symbol->MaxNameLen    = 255;
			symbol->SizeOfStruct  = sizeof( SYMBOL_INFO );

			for(unsigned short i = 0 ; (i < frames) && (stack[i] != nullptr) ; i++)
			{
				SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

				if (strstr(symbol->Name, "DebugTools::DisplayCallStack") == symbol->Name) // ignore current function from the callstack
					continue;

				IMAGEHLP_LINE64 line;
				DWORD           dwDisplacement;

				line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

				if (SymGetLineFromAddr64(process, (DWORD64)(stack[i]), &dwDisplacement, &line))
				{
					stream << "- " << line.FileName << "(" << line.LineNumber << ")    : " << symbol->Name << std::endl;
				}
			}
		}
	}

}; // namespace chaos
