#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/MathTools.h>
#include <chaos/LogTools.h>
#include <chaos/SparseWriteBuffer.h>
#include <chaos/Buffer.h>
#include <chaos/FileTools.h>
#include <chaos/ReferencedObject.h>
#include <chaos/XMLTools.h>
#include <chaos/TiledMap.h>
#include <chaos/BoostTools.h>
#include <chaos/MyGLFWSingleWindowApplication.h> 

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
protected:

	virtual bool OnDraw(glm::ivec2 size) override
	{
		glm::vec4 clear_color(0.0f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		float far_plane = 1000.0f;
		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glViewport(0, 0, size.x, size.y);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);


		return true;
	}

	virtual void Finalize() override
	{
		manager = nullptr;


	}

	virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
	{
		if (!InitializeTiledMapManager())
			return false;


		return true;
	}

	bool InitializeTiledMapManager()
	{
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// create a tiled map
		manager = new chaos::TiledMap::Manager;
		if (manager == nullptr)
			return false;



#if 0
		boost::filesystem::path const & resource_path = application->GetResourcesPath();


		chaos::TiledMap::Map * map = nullptr;

		boost::filesystem::path map_path = resource_path / "Map" / "map.tmx";



		chaos::Buffer<char> buffer1 = chaos::FileTools::LoadFile(map_path, false);
		if (buffer1 != nullptr)
			map = manager.LoadMap(map_path, buffer1);
#endif
		return true;
	}

protected:

	boost::intrusive_ptr<chaos::TiledMap::Manager> manager;
};


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

	return 0;
}


