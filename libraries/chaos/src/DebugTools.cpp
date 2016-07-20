#include <chaos/DebugTools.h>

namespace chaos
{

  // code inspired from OpenGL insights : chapter 33
  void DebugTools::DisplayCallStack(std::ostream & stream)
  {   
    HANDLE process = GetCurrentProcess();
   
    SymSetOptions(SYMOPT_LOAD_LINES);
    SymInitialize(process, NULL, TRUE);

    void * stack[ 200 ];
    memset(stack, 0, sizeof(stack));

    unsigned short frames = CaptureStackBackTrace(0, 200, stack, NULL);

    char symbol_info_buffer[sizeof(SYMBOL_INFO) + 256 * sizeof( char )];
    memset(symbol_info_buffer, 0, sizeof(symbol_info_buffer));

    SYMBOL_INFO  * symbol = (SYMBOL_INFO *)symbol_info_buffer;
    symbol->MaxNameLen    = 255;
    symbol->SizeOfStruct  = sizeof( SYMBOL_INFO );

    for(unsigned short i = 0 ; (i < frames) && (stack[i] != nullptr) ; i++)
    {
      SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

      if (strstr(symbol->Name, "chaos::DebugTools::DisplayCallStack") == symbol->Name) // ignore current function from the callstack
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

}; // namespace chaos
