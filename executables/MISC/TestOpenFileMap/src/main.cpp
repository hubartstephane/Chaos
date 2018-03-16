#include <chaos/StandardHeaders.h>
#include <chaos/Application.h>


int _tmain(int argc, char ** argv, char ** env)
{
  boost::filesystem::path application_path = boost::filesystem::path(argv[0]);
  application_path = application_path.parent_path();

  boost::filesystem::path test_path = application_path / "resources" / "test.txt";

    SYSTEM_INFO SysInfo;
    GetSystemInfo(&SysInfo);

    DWORD dwError = 0;

    boost::timer::cpu_timer timer;

    char const * filename = test_path.string().c_str(); // AIE !!!!

    bool bUseOpen = (argc > 1);


    HANDLE hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(hFile, &FileSize))
        {
            DWORD FileSizeHigh = FileSize.HighPart;
            DWORD FileSizeLow  = FileSize.LowPart;

            HANDLE hFileMapping = NULL;

            if (bUseOpen)
            {
                CloseHandle(hFile);
                hFileMapping = OpenFileMapping(PAGE_READONLY, TRUE, TEXT("MYTOTO"));
            }
            else
                hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0 /*FileSizeHigh*/, 0 /*FileSizeLow*/, TEXT("MYTOTO"));

            if (hFileMapping != NULL)
            {
                LPVOID pBuffer = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0 /*FileSizeLow*/);
                if (pBuffer != NULL)
                {
                    boost::timer::cpu_times elapsed1;
                    boost::timer::cpu_times elapsed2;

                    {
                        boost::timer::cpu_timer t1;

                        char sum = 0;                   
                        for (DWORD j = 0 ; j < FileSizeLow ; ++j)
                            sum += ((char*)pBuffer)[j];

                        elapsed1 = t1.elapsed();
                    }

                    {
                        boost::timer::cpu_timer t2;

                        char sum = 0; 
                        for (DWORD j = 0 ; j < FileSizeLow ; ++j)
                            sum += ((char*)pBuffer)[j];

                        elapsed2 = t2.elapsed();
                    }



                    std::string str = timer.format();
                    std::cout << timer.format() << std::endl;

                    MessageBoxA(NULL, str.c_str(), "Timer", MB_OK); 

                    UnmapViewOfFile(pBuffer);
                }
                else
                    dwError = GetLastError();

                CloseHandle(hFileMapping);
            }
            else
                dwError = GetLastError();
        }
        else
            dwError = GetLastError();

        if (!bUseOpen)
            CloseHandle(hFile);
    }
    else
        dwError = GetLastError();

    if (dwError != 0)
    {
        LPVOID lpMsgBuf = NULL;

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dwError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

        if (lpMsgBuf != NULL)
            MessageBox(NULL, (LPCTSTR)lpMsgBuf, TEXT("Error"), MB_OK); 

    }


  return 0;
}

