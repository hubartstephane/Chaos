#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>


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





int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();



  FreeImage_Initialise();

  std::vector<MONITORINFOEX> monitors = chaos::WinTools::EnumerateMonitors();

  for (size_t i = 0 ; i < monitors.size() ; ++i)
  {
    HDC hDC = CreateDC(monitors[i].szDevice, NULL, NULL, NULL);
    if (hDC != NULL)
    {
      FIBITMAP * bitmap = chaos::WinTools::CaptureWindowToImage(hDC);
      if (bitmap != NULL)
      {
        std::string path = chaos::StringTools::Printf("c:\\temp\\testshu_%d.png", i);

        FreeImage_Save(FIF_PNG, bitmap, path.c_str());  
        FreeImage_Unload(bitmap);
      }
      DeleteDC(hDC);
    }
  }
  



  HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL); // for the PRIMARY
  if (hDC != NULL)
  {
    FIBITMAP * bitmap = chaos::WinTools::CaptureWindowToImage(hDC);
    if (bitmap != NULL)
    {
      FreeImage_Save(FIF_PNG, bitmap, "c:\\temp\\display.png");  
      FreeImage_Unload(bitmap);
    }
    DeleteDC(hDC);
  }





  FreeImage_DeInitialise();

  return 0;
}

