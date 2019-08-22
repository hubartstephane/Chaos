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
		virtual Game * CreateGame();

		/** override */
		virtual bool OnMouseButton(int button, int action, int modifier) override;
		/** override */
		virtual bool OnMouseMove(double x, double y) override;
		/** override */
		virtual bool OnCharEvent(unsigned int c) override;
		/** override */
		virtual bool OnKeyEvent(int key, int scan_code, int action, int modifier) override;

		/** override */
		virtual bool OnDraw(chaos::GPURenderer * in_renderer, glm::ivec2 size) override;
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
		/** override */
		virtual void OnIconifiedStateChange(bool iconified) override;
		/** override */
		virtual void OnFocusStateChange(bool gain_focus) override;
		/** override */
		virtual chaos::box2 GetRequiredViewport(glm::ivec2 const & size) const override;

	protected:

		/** pointer on the game */
		chaos::shared_ptr<Game> game;
	};


	template<typename GAME_TYPE>
	class TypedGameWindow : public GameWindow
	{
	protected:

		/** override */
		virtual Game * CreateGame() override
		{
			return new GAME_TYPE;
		}
	};

}; // namespace death
