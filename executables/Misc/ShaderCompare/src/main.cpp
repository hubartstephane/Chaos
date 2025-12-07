#include "chaos/Chaos.h"

struct ProgramData
{
	chaos::Buffer<char> buffer;

	chaos::Buffer<char> buffer2;

	uLong signature = 0;

	chaos::shared_ptr<chaos::GPUProgram> program;

};

static uLong ComputeBufferSignature(chaos::Buffer<char> buffer)
{
	return crc32(0, (Bytef const *)buffer.data, (uInt)buffer.bufsize);
}

class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, Window);

protected:

	ProgramData LoadProgramAndGetSignature(boost::filesystem::path const & directory, chaos::GPUProgramGenerator::DefinitionSet const & definitions)
	{
		ProgramData result;

		chaos::GPUProgramGenerator generator;

		generator.AddShaderSourceFile(chaos::ShaderType::VERTEX, directory / "vertex_shader.txt");
		generator.AddShaderSourceFile(chaos::ShaderType::FRAGMENT, directory / "pixel_shader.txt");

		result.program = generator.GenProgramObject(definitions);
		if (result.program != nullptr)
		{
			result.buffer = chaos::GLShaderTools::GetProgramBinary(result.program->GetResourceID());
			result.buffer2 = chaos::GLShaderTools::GetProgramBinary(result.program->GetResourceID());
			result.signature = ComputeBufferSignature(result.buffer);
		}
		return result;
	}

	virtual bool Initialize() override
	{
		auto a = sizeof(uLong);
		auto b = sizeof(unsigned long);
		auto c = sizeof(uInt);                                                                     
		auto d = sizeof(size_t);

		chaos::Application const * application = chaos::Application::GetInstance();

		boost::filesystem::path const & resource_path = application->GetResourcesPath();

		chaos::GPUProgramGenerator::DefinitionSet definitions1;	
		ProgramData data1 = LoadProgramAndGetSignature(resource_path / "program1", definitions1);

		chaos::GPUProgramGenerator::DefinitionSet definitions2;
		definitions2["HIDDEN_CODE"] = 0;
		ProgramData data2 = LoadProgramAndGetSignature(resource_path / "program2", definitions2);

		if (data1.buffer.bufsize == data2.buffer.bufsize)
		{
			for (size_t j = 0; j < data1.buffer.bufsize; ++j)
			{
				if (data1.buffer.data[j] != data1.buffer.data[j])
					break;


			}

			//int cp = memcmp(data1.buffer.data, data2.buffer.data, data1.buffer.bufsize);

			int cp = memcmp(data1.buffer.data, data1.buffer2.data, data1.buffer.bufsize);


			cp = cp;

		}

		return true;
	}

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{


		return true;
	}

};

int main(int argc, char ** argv, char ** env)
{

    return chaos::RunSimpleWindowApplication<WindowOpenGLTest>(argc, argv, env);
}
