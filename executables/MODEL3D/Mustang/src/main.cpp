#include "chaos/Chaos.h"


class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, chaos::Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{
		float fov = 60.0f;
		float far_plane = 5000.0f;
		float near_plane = 20.0f;

		glm::vec4 clear_color(0.1f, 0.0f, 0.0f, 0.0f);
		glClearBufferfv(GL_COLOR, 0, (GLfloat*)&clear_color);

		glClearBufferfi(GL_DEPTH_STENCIL, 0, far_plane, 0);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// compute matrices
		glm::mat4x4 projection = glm::perspectiveFov(fov * (float)M_PI / 180.0f, float(draw_params.viewport.size.x), float(draw_params.viewport.size.y), near_plane, far_plane);
		glm::mat4x4 world_to_camera = fps_view_controller.GlobalToLocal();
		glm::mat4x4 local_to_world = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		// prepare unforms
		chaos::GPUProgramProviderChain main_uniform_provider(uniform_provider);
		main_uniform_provider.AddVariable("projection", projection);
		main_uniform_provider.AddVariable("world_to_camera", world_to_camera);
		main_uniform_provider.AddVariable("local_to_world", local_to_world);

		chaos::GPURenderParams render_params;
		mesh_sphere->DisplayWithProgram(program.get(), renderer, &main_uniform_provider, render_params);


		return true;
	}

	virtual bool InitializeFromConfiguration(nlohmann::json const* config) override
	{
		// super
		if (!chaos::Window::InitializeFromConfiguration(config))
			return false;

		chaos::WindowApplication* application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		// compute resource path
		boost::filesystem::path resources_path = application->GetResourcesPath();

		// generate the mesh
		chaos::sphere3   s = chaos::sphere3(glm::vec3(0.0f, 0.0f, 0.0f), 100.0f);
		chaos::GPUMultiMeshGenerator generators;
		generators.AddGenerator(new chaos::GPUSphereMeshGenerator(s, glm::mat4x4(1.0f), 30), mesh_sphere);
		if (!generators.GenerateMeshes())
			return false;

		// generate the program
		chaos::GPUProgramGenerator program_generator;
		program_generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, resources_path / "pixel_shader.txt");
		program_generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, resources_path / "vertex_shader.txt");
		program = program_generator.GenProgramObject();
		if (program == nullptr)
			return false;

		// update the camera speed
		fps_view_controller.config.back_speed = CAMERA_SPEED;
		fps_view_controller.config.down_speed = CAMERA_SPEED;
		fps_view_controller.config.up_speed = CAMERA_SPEED;
		fps_view_controller.config.forward_speed = CAMERA_SPEED;
		fps_view_controller.config.strafe_speed = CAMERA_SPEED;

		fps_view_controller.fps_view.position.y = 30.0f;

		return true;
	}

	virtual void OnDrawImGuiContent() override
	{
		chaos::ImGuiTools::FullViewportWindow("fullscreen", 0, [this]()
		{
			chaos::DrawImGuiVariable(fps_view_controller.fps_view.position);

			ImGui::Text("yaw"); ImGui::SameLine();
			chaos::DrawImGuiVariable(fps_view_controller.fps_view.yaw);

			ImGui::Text("pitch"); ImGui::SameLine();
			chaos::DrawImGuiVariable(fps_view_controller.fps_view.pitch);

			ImGui::Text("roll"); ImGui::SameLine();
			chaos::DrawImGuiVariable(fps_view_controller.fps_view.roll);
		});
		chaos::Window::OnDrawImGuiContent();
	}

	virtual bool DoTick(float delta_time) override
	{
		fps_view_controller.Tick(glfw_window, delta_time);

		return chaos::Window::DoTick(delta_time);
	}

protected:

	static constexpr float CAMERA_SPEED = 400.0f;

	chaos::shared_ptr<chaos::GPUMesh> mesh_sphere;

	chaos::shared_ptr<chaos::GPUProgram> program;

	chaos::FPSViewController fps_view_controller;
};

int main(int argc, char ** argv, char ** env)
{
    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env);
}

















#if 0
virtual void Finalize() override
{
    debug_display.Finalize();
    chaos::Window::Finalize();
}

bool InitializeScene(boost::filesystem::path const& model_path)
{
    chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(model_path);
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

    const aiScene* scene = importer.ReadFileFromMemory(buffer.data, buffer.bufsize, load_flags);


    if (scene == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
    {
        char const* error = importer.GetErrorString();
        if (error != nullptr)
            chaos::Log::Message("Assimp::Importer::ReadFileFromMemory failure [%s]", error);
        return false;
    }

    if (scene->HasMeshes())
    {
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh const* mesh = scene->mMeshes[i];
            if (mesh == nullptr)
                continue;

            i = i;
        }
    }


    if (scene->HasAnimations())
    {
        for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
        {
            aiAnimation const* animation = scene->mAnimations[i];
            if (animation == nullptr)
                continue;

            i = i;

        }
    }

    if (scene->HasTextures())
    {
        for (unsigned int i = 0; i < scene->mNumTextures; ++i)
        {
            aiTexture const* texture = scene->mTextures[i];
            if (texture == nullptr)
                continue;
            i = i;
        }
    }

    if (scene->HasMaterials())
    {
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
        {
            aiMaterial const* material = scene->mMaterials[i];
            if (material == nullptr)
                continue;

            for (unsigned int j = 0; j < material->mNumProperties; ++j)
            {
                aiMaterialProperty* property = material->mProperties[j];
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
            aiCamera const* camera = scene->mCameras[i];
            if (camera == nullptr)
                continue;
            i = i;
        }
    }

    if (scene->HasLights())
    {
        for (unsigned int i = 0; i < scene->mNumLights; ++i)
        {
            aiLight const* light = scene->mLights[i];
            if (light == nullptr)
                continue;
            i = i;
        }
    }

    return ImportSceneNodes(scene, scene->mRootNode);
}

bool ImportSceneNodes(aiScene const* scene, aiNode const* node)
{
    if (node == nullptr)
        return true;

    aiString const& name = node->mName;

    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        unsigned int mesh_index = node->mMeshes[i];

        if (mesh_index >= scene->mNumMeshes)
            continue;

        aiMesh* mesh = scene->mMeshes[mesh_index];
        if (mesh == nullptr)
            continue;
    }

    if (node->mMetaData != nullptr)
    {
        for (unsigned int i = 0; i < node->mMetaData->mNumProperties; ++i)
        {
            aiString        const& key = node->mMetaData->mKeys[i];
            aiMetadataEntry const& value = node->mMetaData->mValues[i];


            i = i;
        }
    }

    bool result = true;
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        result &= ImportSceneNodes(scene, node->mChildren[i]);
        if (!result)
            break;
    }
    return result;
}





virtual bool InitializeFromConfiguration(nlohmann::json const* config) override
{
    if (!chaos::Window::InitializeFromConfiguration(config))
        return false;

    chaos::Application* application = chaos::Application::GetInstance();
    if (application == nullptr)
        return false;

    boost::filesystem::path resources_path = application->GetResourcesPath();

    // load the FONT texture
    boost::filesystem::path image_path = resources_path / "font.png";

    chaos::GLDebugOnScreenDisplay::Params debug_params;
    debug_params.texture_path = image_path;
    debug_params.font_characters = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
    debug_params.font_characters_per_line = 10;
    debug_params.font_characters_line_count = 10;
    debug_params.character_width = 20;
    debug_params.spacing = glm::ivec2(0, 0);
    debug_params.crop_texture = glm::ivec2(15, 7);

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

#endif
