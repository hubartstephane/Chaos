#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	std::streamsize StreamToDebugOutputBuf::xsputn(const char* s, std::streamsize n)
	{
#if _WIN32
		OutputDebugStringA(std::string(s, n).c_str());
#endif
		return n;
	}

	int StreamToDebugOutputBuf::overflow(int c)
	{
#if _WIN32
		if (c != EOF)
		{
			char ch = static_cast<char>(c);
			OutputDebugStringA(std::string(1, ch).c_str());
		}
#endif
		return c;
	}

}; // namespace chaos
