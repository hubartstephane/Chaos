#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/MyGLFWWindow.h>

#include <death/Game.h>

namespace death
{

	class GameWindow : public chaos::MyGLFW::Window
	{

	protected:

		/** game instance creation */
		virtual Game * CreateGameInstance();

		/** override */
		virtual void OnMouseButton(int button, int action, int modifier) override;
		/** override */
		virtual void OnMouseMove(double x, double y) override;
		/** override */
		virtual void OnCharEvent(unsigned int c) override;
		/** override */
		virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override;
		/** override */
		virtual bool OnDraw(glm::ivec2 size) override;
		/** override */
		virtual void Finalize() override;
		/** override */
		virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;
		/** override */
		virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override;
		/** override */
		virtual bool Tick(double delta_time) override;
		/** override */
		virtual void OnInputModeChanged(int new_mode, int old_mode) override;

	protected:

		/** pointer on the game */
		boost::intrusive_ptr<Game> game;
	};


	template<typename GAME_TYPE>
	class TypedGameWindow
	{
	protected:

		/** override */
		virtual Game * CreateGameInstance() override
		{
			return new GAME_TYPE;
		}
	};

}; // namespace death
