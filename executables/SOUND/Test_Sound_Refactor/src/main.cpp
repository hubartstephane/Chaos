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

#include <rapidjson/StringBuffer.h>
#include <rapidjson/Writer.h>


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


  void HandleNodeArray(rapidjson::Document::Array & a, rapidjson::Document::ValueIterator it)
  {
	  rapidjson::Type type = it->GetType();


	  type = type;
  }

  void HandleNodeObject(rapidjson::Document::Object & o, rapidjson::Document::MemberIterator it)
  {
#if 1
	  int i= 0;

	  std::string n = it->name.GetString();
	  rapidjson::Type type = it->value.GetType();

	 
	  if (type == rapidjson::Type::kStringType)
	  {
		  std::string v = it->value.GetString();


		  if (v == "@filename")
		  {
			  ++i;
		  
		  
		  }




		  ++i;
	  }

	  else if (type == rapidjson::Type::kObjectType)
	  {
		  rapidjson::Document::Object child = it->value.GetObject();
		  for (auto it = child.begin() ; it != child.end() ; ++it)
			  HandleNodeObject(child, it);
		  ++i;
	  }
	  else if (type == rapidjson::Type::kArrayType)
	  {
		  rapidjson::Document::Array a = it->value.GetArray();
		  for (auto it = a.begin() ; it != a.end() ; ++it)
			  HandleNodeArray(a, it);

		  ++i;
	  }
	  else if (type == rapidjson::Type::kTrueType)
	  {
		  bool b = it->value.GetBool();
		  ++i;
	  }
	  else if (type == rapidjson::Type::kFalseType)
	  {
		  bool b = it->value.GetBool();
		  ++i;
	  }
	  else if (type == rapidjson::Type::kNullType)
	  {
		  
		  ++i;
	  }
	  else if (type == rapidjson::Type::kNumberType)
	  {
		  if (it->value.IsDouble() || it->value.IsFloat())
		  {
			  float f = it->value.GetFloat();
			  f =f;
		  
		  }
		  if (it->value.IsInt())
		  {
			  int   num = it->value.GetInt();
			  num = num;
		  
		  }

		  
		  
		  //std::string v = it.value.GetString(); // exception !!
		  ++i;
	  }
#endif
  
  }

  virtual bool Initialize(nlohmann::json const & configuration) override
  {
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;


	boost::filesystem::path configuration_path = application->GetResourcesPath() / "config.json";
	chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(configuration_path, true);
	if (buffer != nullptr)
	{
		rapidjson::Document doc;
		doc.Parse(buffer.data);

    size_t erro = doc.GetErrorOffset();

    rapidjson::ParseErrorCode errcode = doc.GetParseError();
    if (errcode == rapidjson::ParseErrorCode::kParseErrorNone)
    {


    }

		if (doc.IsObject())
		{
			rapidjson::Document::Object & o = doc.GetObject();
			for (auto it = o.begin() ; it != o.end() ; ++it)
				HandleNodeObject(o, it);

      rapidjson::Document::MemberIterator it1 = o.FindMember("ccc");
      if (it1 != o.end())
      {
        int i = 0;


        ++i;
      }

      rapidjson::Document::MemberIterator it2 = o.FindMember("value_true");
      if (it2 != o.end())
      {
        

        rapidjson::Value val(rapidjson::kNumberType);
        val.SetInt(1234);



        //rapidjson::Value myob(rapidjson::kObjectType);
        //myob.AddMember("XXX", val, doc.GetAllocator());

        rapidjson::Value & cc = it2->value.SetObject();

        cc.AddMember("XXX", val, doc.GetAllocator());

       // rapidjson::Document::Object & new_ob = 
      //  new_ob.AddMember("xxxxx")


        int i = 0;
        ++i;
      }

		}

		int i = 0;
		if (doc.IsArray())
		{
			rapidjson::Document::Array & a = doc.GetArray();
			for (auto it = a.begin() ; it != a.end() ; ++it)
				HandleNodeArray(a, it);	
		}
		if (doc.IsInt())
			++i;

		rapidjson::StringBuffer b;
		b.Clear();
		rapidjson::Writer<rapidjson::StringBuffer> writer(b);
		doc.Accept(writer);

		char const * bb = strdup( b.GetString() );

		




		++i;
	}










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











	boost::filesystem::path ref = "mareference/bidule/fichier.txt";

	chaos::FilePathParam P1("truc1.xxx.yyy");
	chaos::FilePathParam P2(std::string("truc2.yyy"));
	chaos::FilePathParam P3(boost::filesystem::path("truc3.zzz"));

	chaos::FilePathParam P4("truc1.xxx.yyy", ref);
	chaos::FilePathParam P5(std::string("truc2.yyy"), ref);
	chaos::FilePathParam P6(boost::filesystem::path("truc3.zzz"), ref);




	auto b1 = chaos::FileTools::IsTypedFile(P1, ".xxx");
	auto b2 = chaos::FileTools::IsTypedFile(P1, ".yyy");

	auto R1 = P1.GetResolvedPath();
	auto R2 = P2.GetResolvedPath();
	auto R3 = P3.GetResolvedPath();
	auto R4 = P4.GetResolvedPath();
	auto R5 = P5.GetResolvedPath();
	auto R6 = P6.GetResolvedPath();


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


