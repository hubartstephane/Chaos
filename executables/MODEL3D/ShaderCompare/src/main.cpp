#include "chaos/Chaos.h"

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, Window);

protected:

	virtual bool Initialize() override
	{
		return true;
	}

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{


		return true;
	}

};

int main(int argc, char ** argv, char ** env)
{

    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env);
}
