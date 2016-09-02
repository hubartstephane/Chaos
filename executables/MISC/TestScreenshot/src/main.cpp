#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>
#include <chaos/FileTools.h>
#include <chaos/Stringtools.h>

#if 0

BOOL WINAPI EnumWindowCallback(HWND hWnd, LPARAM lParam)
{
  std::vector<std::string> * window_names = (std::vector<std::string>*)(lParam);

  TCHAR buf[1024];
  GetWindowText(hWnd, buf, sizeof(buf));

  if (strlen(buf) > 0)
    window_names->push_back(std::string(buf));    
  return TRUE;
}

std::vector<std::string> GetAllWindowNames()
{
  std::vector<std::string> result;
  EnumWindows(EnumWindowCallback, (LPARAM)&result);
  std::sort(result.begin(), result.end());
  auto e = std::unique(result.begin(), result.end());
  result.erase(e, result.end());
  return result;
}

void HandleConsoleEvents()
{
  DWORD NumRead = 0; 
  INPUT_RECORD ConsoleEvents[128]; 

  if (ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), ConsoleEvents, 128, &NumRead) )
  {
    for (DWORD i = 0 ; i < NumRead ; ++i)
    {
      switch(ConsoleEvents[i].EventType) 
      { 
      case KEY_EVENT:
        i = i;
        break; 
      case MOUSE_EVENT:
        i = i;
        break; 
      case WINDOW_BUFFER_SIZE_EVENT:
        i = i;
        break; 
      case FOCUS_EVENT:
        i = i;
        break;
      case MENU_EVENT:
        i = i;
        break; 
      default: 
        break; 
      } 
    }
  }
}

#endif

void CaptureAndSaveMonitor(boost::filesystem::path const & dst_p, char const * monitor_name, char const * filename)
{
  HDC hDC = CreateDC(monitor_name, NULL, NULL, NULL);
  if (hDC != NULL)
  {
    FIBITMAP * bitmap = chaos::WinTools::CaptureWindowToImage(hDC);
    if (bitmap != NULL)
    {
      FreeImage_Save(FIF_PNG, bitmap, (dst_p / filename).string().c_str());  
      FreeImage_Unload(bitmap);
    }
    DeleteDC(hDC);
  }
}

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  FreeImage_Initialise();

  boost::filesystem::path dst_p;
  if (chaos::FileTools::CreateTemporaryDirectory("ScreenShot", dst_p))
  {
    std::vector<MONITORINFOEX> monitors = chaos::WinTools::EnumerateMonitors();
    for (size_t i = 0 ; i < monitors.size() ; ++i)
      CaptureAndSaveMonitor(dst_p, monitors[i].szDevice, chaos::StringTools::Printf("Monitor%d.png", i).c_str());
    CaptureAndSaveMonitor(dst_p, "DISPLAY", "MainMonitor.png");
  
    chaos::WinTools::ShowFile(dst_p.string().c_str());
  }

  FreeImage_DeInitialise();

  return 0;
}

