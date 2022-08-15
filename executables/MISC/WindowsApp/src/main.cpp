#include <chaos/Chaos.h>

class MyWindow : public chaos::SimpleWin32Window
{
public:

	virtual LRESULT OnLButtonDown(int x, int y, int buttonStates)
	{
		//::ShowWindow(GetHwnd(), SW_HIDE);

		FIBITMAP * bitmap = chaos::WinTools::CaptureWindowToImage(GetHwnd());
		if (bitmap != NULL)
		{
			std::string path = chaos::StringTools::Printf("c:\\temp\\capture_shu.png");

			FreeImage_Save(FIF_PNG, bitmap, path.c_str());  
			FreeImage_Unload(bitmap);
		}

		//::ShowWindow(GetHwnd(), SW_SHOW);

		return 0;
	}

	virtual LRESULT OnWindowSize(int width, int height) override
	{
		InvalidateRect(GetHwnd(), nullptr, true);
		return SimpleWin32Window::OnWindowSize(width, height);
	}

	virtual LRESULT OnPaint() override
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		int division = 100;

		for (int i = 0 ; i < division ; ++i)
		{
			float r = 255.0f * ((float) i) / (float)(division - 1);

			COLORREF color = RGB((INT)r, 0, 0);

			HBRUSH hBrush = CreateSolidBrush(color);
			if (hBrush != NULL)
			{

				float division_size = (float)(ps.rcPaint.bottom - ps.rcPaint.top) / (float)division;

				float top    = division_size * (float)i;
				float bottom = division_size * (float)(i + 1);

				RECT rect;        
				rect.left   = ps.rcPaint.left;
				rect.right  = ps.rcPaint.right;
				rect.top    = (LONG)top;
				rect.bottom = (LONG)bottom;

				FillRect(hdc, &rect, hBrush);
				DeleteObject(hBrush);
			}
		}


		EndPaint(hWnd, &ps);
		return 0;
	}

	virtual LRESULT OnEraseBackground(HDC hDC) override
	{
		return 1;
	}

};

//
// Test : ATTENTION ceci montre que si une fenetre sort du cadre de l'ecran, elle est capturee correctement
//        Vrai au taffe ... Pas vrai a la maison !!!!!
//
// Test : en revanche, si on encapsule la capture avec 
//   ::ShowWindow(GetHwnd(), SW_HIDE); 
//   ...
//   ::ShowWindow(GetHwnd(), SW_SHOW);
//
//  la capture retourne un ecran noir

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		chaos::SimpleWin32CreateParam create_params;
		create_params.x         = 10;
		create_params.y         = 10;
		create_params.nWidth    = 300;
		create_params.nHeight   = 2000;
		create_params.dwExStyle = 0;
		//  create_params.dwStyle   = WS_POPUP;

		chaos::SimpleWin32Class<MyWindow> c("class1");
		c.CreateWindowHinstance(NULL, "my_window", create_params);
		chaos::SimpleWin32Window::SimpleMessageLoop();		return true;
	}
};

int main(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


