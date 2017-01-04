#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MIDITools.h>

class BufferReader
{
public:

	/** constructor */
	BufferReader(chaos::Buffer<char> const & in_buffer) : buffer(in_buffer){}
	/** test whether eof is reached */
	bool IsEOF() const { return position >= buffer.bufsize; }
	/** returns true whether there is enough data in buffer to read wanted value */
	bool IsEnoughData(size_t size) const { return (buffer.bufsize - position) >= size; }
	/** read data for one POD object */
	template<typename T>
	bool Read(T & result)
	{
		return ReadN(&result, 1);
	}
	/** read data for several POD object */
	template<typename T>
	bool ReadN(T * result, size_t count = 1)
	{
		if (!IsEnoughData(sizeof(T) * count))
			return false;
		memcpy(result, &buffer[position], sizeof(T) * count);
		position += sizeof(T) * count;
		return true;
	}
	
protected:

	/** the position in the buffer */
	size_t position{0};
	/** the reference of the buffer where to stream data */
	chaos::Buffer<char> const & buffer;
};

class MidiChunk : public chaos::Buffer<char>
{
public:

	/** returns true whether the chunk is a header chunk */
	bool IsHeaderChunk() const { return IsTypedTrackChunk("MThd");}
	/** returns true whether the chunk is a track chunk */
	bool IsTrackChunk() const { return IsTypedTrackChunk("MTrk");}
	/** utility method to test for chunk name */
	bool IsTypedTrackChunk(char const * type_name) const {return (strncmp(chunk_name, type_name, 4) == 0); }

public:

	char chunk_name[5] = {0, 0, 0, 0, 0};
};

class MidiLoader
{
public:




	bool LoadBuffer(chaos::Buffer<char> const & buffer);

protected:

	/** clean the content thata have parsed */
	void Clean();
	/** the effective method to read the buffer */
	bool DoLoadBuffer(BufferReader & reader);

	/** read a sub chunk of data */
	MidiChunk ReadChunk(BufferReader & reader);
	/** read the header chunk */
	MidiChunk ReadHeaderChunk(BufferReader & reader);
};

MidiChunk MidiLoader::ReadChunk(BufferReader & reader)
{
	MidiChunk result;
	if (reader.IsEnoughData(8))
	{
		// read the chunk name
		reader.ReadN(result.chunk_name, 4);
		// read the size of the data
		uint32_t data_size;
		reader.Read(data_size);
		
		if (reader.IsEnoughData(data_size))
		{
		
			data_size = data_size;
		}
		
	
	}
	return result;
}

MidiChunk MidiLoader::ReadHeaderChunk(BufferReader & reader)
{
	MidiChunk result = ReadChunk(reader);
	if (result.IsHeaderChunk())
		return result;
	return MidiChunk();
}

bool MidiLoader::LoadBuffer(chaos::Buffer<char> const & buffer)
{
	Clean();
	if (!DoLoadBuffer(BufferReader(buffer)))
	{
		Clean();
		return false;
	}
	return true;
}

void MidiLoader::Clean()
{

}

bool MidiLoader::DoLoadBuffer(BufferReader & reader)
{
	MidiChunk chunk = ReadHeaderChunk(reader);
	if (chunk == nullptr)
		return false;



	return true;
}


// ================================================================


class MyGLFWWindowOpenGLTest1 : public chaos::MyGLFWWindow
{
	friend class MIDICommandEvent;

protected:

	virtual bool OnDraw(int width, int height) override
	{
		glClearColor(0.0f, 0.0, 0.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return true;
	}



	virtual bool Initialize() override
	{
		chaos::Application * application = chaos::Application::GetInstance();
		if (application == nullptr)
			return false;

		boost::filesystem::path const & resource_path = application->GetResourcesPath();

		chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(resource_path / "Blues Breaker - 8 Bars" / "BluesBreaker_8Bars_01.mid", false);
		if (buffer == nullptr)
			return false;

		MidiLoader loader;
		return loader.LoadBuffer(buffer);	
	}


	virtual void Finalize() override
	{

	}


	virtual void TweakSingleWindowApplicationHints(chaos::MyGLFWWindowHints & hints, GLFWmonitor * monitor, bool pseudo_fullscreen) const override
	{
		chaos::MyGLFWWindow::TweakSingleWindowApplicationHints(hints, monitor, pseudo_fullscreen);
		hints.toplevel = 0;
		hints.decorated = 1;
	}
};

// ================================================================


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();

	chaos::MyGLFWSingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::MyGLFWWindow::RunSingleWindowApplication<MyGLFWWindowOpenGLTest1>(params);

	return 0;
}


