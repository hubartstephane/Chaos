#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/Application.h>
#include <chaos/MyGLFWWindow.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 

#include "LudumGame.h"


class LudumWindow : public chaos::MyGLFW::Window
{
	static float const VIEWPORT_WANTED_ASPECT;

protected:

	virtual void OnMouseButton(int button, int action, int modifier) override;

	virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override;

	virtual bool OnDraw(glm::ivec2 size) override;

	virtual void Finalize() override;

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override;

	virtual bool Tick(double delta_time) override;

	virtual void OnInputModeChanged(int new_mode, int old_mode) override;

protected:

	boost::intrusive_ptr<LudumGame> game;
};