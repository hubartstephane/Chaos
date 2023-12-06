#include "chaos/Chaos.h"

class MyWindow : public chaos::SimpleWin32Window
{
public:

	virtual LRESULT OnKeyDown(int keycode, int scancode) override
	{
		unsigned int mapped_key = ::MapVirtualKey(scancode, MAPVK_VSC_TO_VK);

		std::string name = chaos::KeyboardLayout::ScancodeToName(scancode);
		TextToDisplay = chaos::StringTools::Printf("keycode = [0x%03x]    scancode= [0x%03x]    mapped_key= [0x%03x]     name = [%s]", keycode, scancode, mapped_key, name.c_str());
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}

	LRESULT OnDestroy() override
	{
		if (hFont != NULL)
		{
			DeleteObject(hFont);
			hFont = NULL;
		}
		return chaos::SimpleWin32Window::OnDestroy();
	}

	virtual LRESULT OnCreate(CREATESTRUCTA* create_param) override
	{
		hFont = CreateFont(
			30, 0, 0, 0,
			FW_DONTCARE,
			FALSE,
			FALSE,
			FALSE,
			ANSI_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			"Arial");

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

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_DESKTOP + 1));

		RECT rect;
		GetClientRect(hWnd, &rect);
		SelectFont(hdc, hFont);
		DrawText(hdc, TextToDisplay.c_str(), (int)TextToDisplay.length(), &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); // DT_VCENTER only works with DT_SINGLELINE

		EndPaint(hWnd, &ps);
		return 0;
	}

protected:

	std::string TextToDisplay;

	HFONT hFont = NULL;

};

class MyApplication : public chaos::Application
{
protected:

	virtual int Main() override
	{
		chaos::SimpleWin32CreateParam create_params;
		create_params.x         = 10;
		create_params.y         = 10;
		create_params.nWidth    = 1100;
		create_params.nHeight   = 300;
		create_params.dwExStyle = 0;
		//  create_params.dwStyle   = WS_POPUP;

		chaos::SimpleWin32Class<MyWindow> c("class1");
		c.CreateWindowHinstance(NULL, "my_window", create_params);
		chaos::SimpleWin32Window::SimpleMessageLoop();
		return 0;
	}
};

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunApplication<MyApplication>(argc, argv, env);
}