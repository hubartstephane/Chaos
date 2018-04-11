#pragma once

#include <chaos/StandardHeaders.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/CollisionFramework.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/GLTextureTools.h>
#include <chaos/MyGLFWGamepadManager.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLDebugOnScreenDisplay.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GPUProgramData.h>
#include <chaos/GPUProgram.h>
#include <chaos/Texture.h>
#include <chaos/VertexDeclaration.h>
#include <chaos/GPUProgramProvider.h>
#include <chaos/SoundManager.h>
#include <json.hpp>
#include <chaos/BoostTools.h>
#include <chaos/BitmapAtlas.h>
#include <chaos/BitmapAtlasGenerator.h>
#include <chaos/TextureArrayAtlas.h>
#include <chaos/SpriteManager.h>
#include <chaos/JSONTools.h>

#include "LudumGame.h"


class LudumWindow : public chaos::MyGLFW::Window
{

	static float const VIEWPORT_WANTED_ASPECT;

protected:

	virtual void OnKeyEvent(int key, int scan_code, int action, int modifier) override;

	virtual bool OnDraw(glm::ivec2 size) override;

	virtual void Finalize() override;

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override;

	virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override;

	virtual bool Tick(double delta_time) override;

protected:

	boost::intrusive_ptr<LudumGame> game;
};