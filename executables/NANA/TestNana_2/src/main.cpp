#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>



using namespace nana;

class notepad_form
	: public form
{
public:
	notepad_form()
	{
		caption("Simple Notepad - Nana C++ Library");
		menubar_.create(*this);

		textbox_.create(*this);
		textbox_.borderless(true);
		API::effects_edge_nimbus(textbox_, effects::edge_nimbus::none);
		textbox_.enable_dropfiles(true);
		textbox_.events().mouse_dropfiles([this](const arg_dropfiles& arg)
		{
			if (arg.files.size() && _m_ask_save())
				textbox_.load(arg.files.at(0).data());
		});

		_m_make_menus();

		place_.bind(*this);
		place_.div("vert<menubar weight=28><textbox>");
		place_.field("menubar") << menubar_;
		place_.field("textbox") << textbox_;
		place_.collocate();

		events().unload([this](const arg_unload& arg) {
			if (!_m_ask_save())
				arg.cancel = true;
		});
	}
private:
	std::string _m_pick_file(bool is_open) const
	{
		filebox fbox(*this, is_open);
		fbox.add_filter("Text", "*.txt");
		fbox.add_filter("All Files", "*.*");

		return (fbox.show() ? fbox.file() : std::string());
	}

	bool _m_ask_save()
	{
		if (textbox_.edited())
		{
			std::string fs = textbox_.filename();
			msgbox box(*this, "Simple Notepad", msgbox::button_t::yes_no_cancel);
			box << "Do you want to save these changes?";

			switch (box.show())
			{
			case msgbox::pick_yes:
				if (fs.empty())
				{
					fs = _m_pick_file(false);
					if (fs.empty())
						break;
					if (fs.find(".txt") == fs.npos)
						fs += ".txt";
				}
				textbox_.store(fs.data());
				break;
			case msgbox::pick_no:
				break;
			case msgbox::pick_cancel:
				return false;
			}
		}
		return true;
	}

	void _m_make_menus()
	{
		menubar_.push_back("&FILE");
		menubar_.at(0).append("New", [this](menu::item_proxy& ip)
		{
			if (_m_ask_save())
				textbox_.reset();
		});
		menubar_.at(0).append("Open", [this](menu::item_proxy& ip)
		{
			if (_m_ask_save())
			{
				std::string fs = _m_pick_file(true);
				if (fs.size())
					textbox_.load(fs.data());
			}
		});
		menubar_.at(0).append("Save", [this](menu::item_proxy&)
		{
			std::string fs = textbox_.filename();
			if (fs.empty())
			{
				fs = _m_pick_file(false);
				if (fs.empty())
					return;
			}
			textbox_.store(fs.data());
		});

		menubar_.push_back("F&ORMAT");
		menubar_.at(1).append("Line Wrap", [this](menu::item_proxy& ip)
		{
			textbox_.line_wrapped(ip.checked());
		});
		menubar_.at(1).check_style(0, menu::checks::highlight);
	}
private:
	place   place_;
	menubar menubar_;
	textbox textbox_;
};

int _tmain(int argc, char ** argv, char ** env)
{
  notepad_form npform;
  npform.show();
  exec();

  return 0;
}


