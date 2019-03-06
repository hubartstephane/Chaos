#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/LogTools.h> 
#include <chaos/GLTools.h> 
#include <chaos/MyGLFWSingleWindowApplication.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/GPUProgramGenerator.h>
#include <chaos/Application.h>
#include <chaos/FPSViewInputController.h>
#include <chaos/MyFbxImporter.h>
#include <chaos/GPUProgramData.h>
#include <chaos/SimpleMesh.h>


static int const MAX_BONE_COUNT = 10;
static int const SKELETAL_BONE_COUNT = 250;


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFW::Window
{
public:

  MyGLFWWindowOpenGLTest1()
  {
    memset(program, 0, sizeof(GLuint) * MAX_BONE_COUNT);
  }

protected:

  virtual void OnKeyEvent(int key, int scan_code, int action, int modifier)
  {
    if (key == GLFW_KEY_F2 && action == GLFW_RELEASE)
    {
      GLint mode = 0;
      glGetIntegerv(GL_POLYGON_MODE, &mode);
      if (mode == GL_LINE)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); 
      else if (mode == GL_POINT)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
      else if (mode == GL_FILL)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);     
    }  
  }

  virtual bool OnDraw(chaos::Renderer * renderer, glm::ivec2 size) override
  {
    glm::vec4 clear_color(0.2f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 5000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glViewport(0, 0, size.x, size.y);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);   // when viewer is inside the cube
   
    
    for (auto const & mesh : imported_data.meshes)
    {
      int bone_count = mesh->declaration.GetBoneCount();
      if (bone_count < 0 || bone_count >= MAX_BONE_COUNT)
        continue;
           
      glUseProgram(program[bone_count]);

      // XXX : the scaling is used to avoid the near plane clipping      
      static float FOV =  60.0f;
      glm::mat4 projection_matrix = glm::perspectiveFov(FOV * (float)M_PI / 180.0f,(float)size.x, (float)size.y, 1.0f, far_plane);

      glm::mat4 local_to_world_matrix = glm::mat4(1.0);

      glm::mat4 world_to_camera_matrix = fps_view_controller.GlobalToLocal();

      program_data[bone_count].SetUniform("projection",      projection_matrix);
      program_data[bone_count].SetUniform("local_to_world",  local_to_world_matrix);
      program_data[bone_count].SetUniform("world_to_camera", world_to_camera_matrix);

      if (bone_count > 0)
      {
        chaos::GLUniformInfo const * info = program_data[bone_count].FindUniform("bone_transforms");
        if (info != nullptr)
        {
          glm::mat4 * m = new glm::mat4[SKELETAL_BONE_COUNT];
          if (m != nullptr)
          {          
            for (int i = 0 ; i < SKELETAL_BONE_COUNT ; ++i)
              m[i] = glm::mat4(1.0f);

            glUniformMatrix4fv(info->location, SKELETAL_BONE_COUNT, false, (float const *)m);
            delete [] m;     
          }                                   
        }     
      }

      float color = (float)(&mesh - &imported_data.meshes[0]) / (float)imported_data.meshes.size();
      program_data[bone_count].SetUniform("mesh_color", color);
      mesh->Render(program_data[bone_count], nullptr);  
    }

    return true;
  }

  virtual void Finalize() override
  {
    for (int i = 0 ; i < MAX_BONE_COUNT ; ++i)
      if (program[i] != 0)
        glDeleteProgram(program[i]);

    imported_data.Clean();
  }

  virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path) override
  {   
    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetResourcesPath();

    // create shader
    chaos::GPUProgramGenerator program_generator;
    program_generator.AddShaderSourceFile(GL_FRAGMENT_SHADER, resources_path / "pixel_shader.txt");
    program_generator.AddShaderSourceFile(GL_VERTEX_SHADER,   resources_path / "vertex_shader.txt");

    chaos::GPUProgramGenerator::DefinitionSet definitions;
    definitions["SKELETAL_BONE_COUNT"] = SKELETAL_BONE_COUNT;   

    for (int i = 0 ; i < MAX_BONE_COUNT ; ++i)
    {
      definitions["BONE_COUNT"] = i;   

      program[i] = loader.GenProgram(definitions);
      if (program[i] == 0)
        return false;
    
      program_data[i] = chaos::GPUProgramData::GetData(program[i]);
    }

    // Load the model
    static int MODEL_INDEX = 0;

    boost::filesystem::path model_path;
    if (MODEL_INDEX % 3 == 0)
      model_path = resources_path / "AG_CIM_Act_Stand_Drink_1_2.fbx";
    else if (MODEL_INDEX % 3 == 1)
      model_path = resources_path / "house.fbx";
    else if (MODEL_INDEX % 3 == 2)
      model_path = resources_path / "watcher.fbx";
    
    chaos::MyFbxImporterParams params;
    params.vertex_transforms = glm::mat4(0.05f);

    chaos::MyFbxImporter importer;
    if (!importer.ImportScene(model_path, imported_data, params))
      return false;
 
    if (imported_data.meshes.size() == 0)
      return false;
   
    return true;
  }

  virtual void TweakHints(chaos::MyGLFW::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::MyGLFW::Window::TweakHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 1;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    if (glfwGetKey(glfw_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      RequireWindowClosure();

    fps_view_controller.Tick(glfw_window, delta_time);

    return true; // refresh
  }

protected:

  GLuint program[MAX_BONE_COUNT];

  chaos::GPUProgramData program_data[MAX_BONE_COUNT];
  
  chaos::FPSViewInputController fps_view_controller;

  chaos::MyFbxImporterOutput imported_data;
}; 

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::MyGLFW::SingleWindowApplicationParams params;
  params.monitor = nullptr;
  params.width = 500;
  params.height = 500;
  params.monitor_index = 0;
  chaos::MyGLFW::RunWindowApplication<MyGLFWWindowOpenGLTest1>(argc, argv, env, params);

  return 0;
}


