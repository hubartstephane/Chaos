#include <chaos/Chaos.h>


class WindowOpenGLTest : public chaos::Window
{
    CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

  virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::WindowDrawParams const& draw_params, chaos::GPUProgramProviderInterface const * uniform_provider) override
  {
    glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
    glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

    float far_plane = 1000.0f;
    glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);   // when viewer is inside the cube

    return true;
  }

  virtual void Finalize() override
  {
    debug_display.Finalize();
		chaos::Window::Finalize();
  }

  bool InitializeScene(boost::filesystem::path const & model_path)
  {
    chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(model_path, LoadFileFlag::NONE);
    if (buffer == nullptr)
      return false;



    Assimp::Importer importer;

    std::string supported_extension;
    importer.GetExtensionList(supported_extension);


    importer.SetExtraVerbose(true);


    unsigned int load_flags =
      aiProcess_CalcTangentSpace |
      aiProcess_Triangulate |
      aiProcess_JoinIdenticalVertices |
      aiProcess_SortByPType;

    const aiScene * scene = importer.ReadFileFromMemory(buffer.data, buffer.bufsize, load_flags);


    if (scene == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
    {
      char const * error = importer.GetErrorString();
      if (error != nullptr)
        chaos::Log::Message("Assimp::Importer::ReadFileFromMemory failure [%s]", error);
      return false;
    }

    if (scene->HasMeshes())
    {
      for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
      {
        aiMesh const * mesh = scene->mMeshes[i];
        if (mesh == nullptr)
          continue;

        i = i;
      }
    }


    if (scene->HasAnimations())
    {
      for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
      {
        aiAnimation const * animation = scene->mAnimations[i];
        if (animation == nullptr)
          continue;

        i = i;

      }
    }

    if (scene->HasTextures())
    {
      for (unsigned int i = 0; i < scene->mNumTextures; ++i)
      {
        aiTexture const * texture = scene->mTextures[i];
        if (texture == nullptr)
          continue;
        i = i;
      }
    }

    if (scene->HasMaterials())
    {
      for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
      {
        aiMaterial const * material = scene->mMaterials[i];
        if (material == nullptr)
          continue;

        for (unsigned int j = 0; j < material->mNumProperties; ++j)
        {
          aiMaterialProperty * property = material->mProperties[j];
          if (property == nullptr)
            continue;
          j = j;
        }
      }
    }

    if (scene->HasCameras())
    {
      for (unsigned int i = 0; i < scene->mNumCameras; ++i)
      {
        aiCamera const * camera = scene->mCameras[i];
        if (camera == nullptr)
          continue;
        i = i;
      }
    }

    if (scene->HasLights())
    {
      for (unsigned int i = 0; i < scene->mNumLights; ++i)
      {
        aiLight const * light = scene->mLights[i];
        if (light == nullptr)
          continue;
        i = i;
      }
    }

    return ImportSceneNodes(scene, scene->mRootNode);
  }

  bool ImportSceneNodes(aiScene const * scene, aiNode const * node)
  {
    if (node == nullptr)
      return true;

    aiString const & name = node->mName;

    for (unsigned int i = 0 ; i < node->mNumMeshes ; ++i)
    {
      unsigned int mesh_index = node->mMeshes[i];

      if (mesh_index >= scene->mNumMeshes)
        continue;

      aiMesh * mesh = scene->mMeshes[mesh_index];
      if (mesh == nullptr)
        continue;
    }

    if (node->mMetaData != nullptr)
    {
      for (unsigned int i = 0 ; i < node->mMetaData->mNumProperties ; ++i)
      {
        aiString        const & key = node->mMetaData->mKeys[i];
        aiMetadataEntry const & value = node->mMetaData->mValues[i];


        i = i;
      }
    }

    bool result = true;
    for (unsigned int i = 0 ; i < node->mNumChildren ; ++i)
    {
      result &= ImportSceneNodes(scene, node->mChildren[i]);
      if (!result)
        break;
    }
    return result;
  }





  virtual bool InitializeFromConfiguration(nlohmann::json const & config) override
  {
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

    chaos::Application * application = chaos::Application::GetInstance();
    if (application == nullptr)
      return false;

    boost::filesystem::path resources_path = application->GetResourcesPath();

    // load the FONT texture
    boost::filesystem::path image_path     = resources_path / "font.png";

    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path               = image_path;
    debug_params.font_characters            = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line   = 10;
    debug_params.font_characters_line_count = 10;
    debug_params.character_width            = 20;
    debug_params.spacing                    = glm::ivec2( 0, 0);
    debug_params.crop_texture               = glm::ivec2(15, 7);

    if (!debug_display.Initialize(debug_params))
      return false;

    debug_display.AddLine("HelloWorld");

    // Load the model
    static int MODEL_INDEX = 1;

    boost::filesystem::path model_path;
    if (MODEL_INDEX % 3 == 0)
      model_path = resources_path / "AG_CIM_Act_Stand_Drink_1_2.fbx";
    else if (MODEL_INDEX % 3 == 1)
      model_path = resources_path / "house.fbx";
    else if (MODEL_INDEX % 3 == 2)
      model_path = resources_path / "watcher.fbx";

    if (!InitializeScene(model_path))
      return false;

    return true;
  }

  virtual void TweakHints(chaos::WindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
  {
    chaos::Window::TweakHints(hints, monitor, pseudo_fullscreen);

    hints.toplevel  = 1;
    hints.decorated = 1;
  }

  virtual bool Tick(double delta_time) override
  {
    fps_view_controller.Tick(glfw_window, delta_time);

    debug_display.Tick(delta_time);

    return true; // refresh
  }

protected:

  chaos::FPSViewInputController fps_view_controller;

  chaos::GLDebugOnScreenDisplay debug_display;
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
    chaos::WindowParams params;
    params.monitor = nullptr;
    params.width = 500;
    params.height = 500;
    params.monitor_index = 0;

    chaos::WindowHints hints;

    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env, params, hints);
}


