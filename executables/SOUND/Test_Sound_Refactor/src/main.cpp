#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MathTools.h>
#include <chaos/SoundManager.h>

#include "SoundTest.h"

class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{

protected:

  virtual bool OnDraw(glm::ivec2 size) override
  {
    glClearColor(0.0f, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return true;
  }

  virtual void Finalize() override
  {   

  }

  virtual bool Tick(double delta_time) override
  {

    return false; // no redraw
  }

  virtual void OnMouseButton(int button, int action, int modifier) override
  {



  }

  virtual bool Initialize(nlohmann::json const & configuration) override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;


    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);
    hints.toplevel = 0;
    hints.decorated = 1;
  }

protected:


};

// ===============================================

class A
{
public:

	A(int i):value(i)
	{
		p = new int(5);	
	}

	int value = 0;

	int * p = nullptr;

	void F() const
	{
		*p = 667;
	}
};

class PARAM
{
public:

	PARAM(int u){}

	PARAM(A & in_a) : a(&in_a){}

	A * a = nullptr;
};



void F(PARAM const & p)
{
	if (p.a != nullptr)
		p.a->F();


}

void G(PARAM const & p)
{
	F(p);

}



void TT(chaos::FilePathParam const & p)
{
	chaos::FileTools::LoadFile(p, true);
}





// ===============================================


int _tmain(int argc, char ** argv, char ** env)
{
	TT("truc");
	TT(std::string("truc"));
	TT(boost::filesystem::path("truc"));

	chaos::FilePath pp;
	TT(pp);

	G(5);

	A a(6);
	G(a);

  ParticleLayer sl(new ParticleLayerDescExample());

  ParticleRange r = sl.SpawnParticles(17);

  boost::intrusive_ptr<ParticleRangeAllocation> p1 = sl.SpawnParticlesAndKeepRange(5);
  boost::intrusive_ptr<ParticleRangeAllocation> p2 = sl.SpawnParticlesAndKeepRange(10);
  boost::intrusive_ptr<ParticleRangeAllocation> p3 = sl.SpawnParticlesAndKeepRange(20);

  p2 = nullptr;
  sl.TickParticles(0.0f);

  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


