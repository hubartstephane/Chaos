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

		void LogSourceLocation(char const* in_message, std::source_location const& in_location)
		{
			static bool first_call = true; // this is a debug function. Don't care if this is clean code
			static std::string filepath;

			// compute the filepath
			if (first_call)
			{
				Application const* application = Application::GetInstance(); // this function could be called before the initialization of the application. need to check
				if (application == nullptr)
					return;
				filepath = (application->GetUserLocalTempPath() / "log_source_locations.txt").string();
			}

			// open the file and log info
			FILE* file = NULL;
			fopen_s(&file, filepath.c_str(), first_call ? "w" : "a"); // clean the file for first call
			if (file != NULL)
			{
				std::string location;

				// make the string location more readable
				char const * function_name = in_location.function_name();
				char const * end_markup = strchr(function_name, '(');
				if (end_markup == nullptr || end_markup == function_name)
				{
					location = function_name;
				}
				else
				{
					char const* start_markup = end_markup - 1; // can go before, because checked above
					while (start_markup >= function_name)
					{
						if (*start_markup != ':' && !StringTools::IsVariableCharacter(*start_markup))
							break;
						--start_markup;
					}
					location = std::string(start_markup + 1, end_markup);
				}

				if (in_message != nullptr)
					fprintf(file, "%s(%d)  msg = [%s]   file = [%s]\r\n", location.c_str(), in_location.line(), in_message, in_location.file_name());
				else
					fprintf(file, "%s(%d)    file = [%s]\r\n", location.c_str(), in_location.line(), in_location.file_name());
				fclose(file);
			}
			// start the application that shows .txt files
			if (first_call)
				WinTools::ShowFile(filepath.c_str());

			first_call = false;
		}

	}; // namespace DebugTools

}; // namespace chaos
