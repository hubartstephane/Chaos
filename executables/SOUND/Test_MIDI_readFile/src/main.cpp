#include <chaos/StandardHeaders.h> 
#include <chaos/FileTools.h> 
#include <chaos/Buffer.h> 
#include <chaos/LogTools.h> 
#include <chaos/MyGLFWWindow.h> 
#include <chaos/WinTools.h> 
#include <chaos/Application.h>
#include <chaos/IrrklangTools.h>
#include <chaos/MIDITools.h>
#include <chaos/EndianTools.h>



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
	/** get a pointer on the current buffer at position */
	char const * GetCurrentPosition() const { return &buffer.data[position];}

	/** advance current pointer */
	void Advance(size_t offset)
	{
		position += offset;
		assert(position <= buffer.bufsize);	
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
	/** the name for the chunk */
	char chunk_name[5] = {0, 0, 0, 0, 0};
};


class MidiEvent
{
public:

	MidiEvent(unsigned char in_signature) : signature(in_signature){}

	virtual ~MidiEvent() = default;

	virtual bool InitializeEventFromChunk(BufferReader & reader){ return false;}

public:

	unsigned char signature{0};
};

class MidiSystemExclusiveEvent : public MidiEvent
{
public:

	MidiSystemExclusiveEvent(unsigned char in_signature) : MidiEvent(in_signature){}

	virtual bool InitializeEventFromChunk(BufferReader & reader) override;
};

class MidiMetaEvent : public MidiEvent
{
public:

	MidiMetaEvent(unsigned char in_signature) : MidiEvent(in_signature){}

	virtual bool InitializeEventFromChunk(BufferReader & reader) override;
};

class MidiCommandEvent : public MidiEvent
{
public:

	MidiCommandEvent(unsigned char in_signature) : MidiEvent(in_signature){}

	virtual bool InitializeEventFromChunk(BufferReader & reader) override;
};

bool MidiSystemExclusiveEvent::InitializeEventFromChunk(BufferReader & reader)
{

	return true;
}

bool MidiMetaEvent::InitializeEventFromChunk(BufferReader & reader)
{

	return true;
}

bool MidiCommandEvent::InitializeEventFromChunk(BufferReader & reader)
{

	return true;
}













class MidiTrack
{
public:

	virtual ~MidiTrack() = default;

public:

	/** the events */
	std::vector<std::unique_ptr<MidiEvent>> events;
};

class MidiHeader
{
public:

	/** returns true whether the header is correct */
	bool IsValid() const { return (format == FORMAT_SINGLE_TRACK) || (format == FORMAT_MULTIPLE_TRACK) || (format == FORMAT_MULTIPLE_SONG);}	

public:

	static int16_t const FORMAT_SINGLE_TRACK = 0;
	static int16_t const FORMAT_MULTIPLE_TRACK = 1;
	static int16_t const FORMAT_MULTIPLE_SONG  = 2;

	int16_t format{0};
	int16_t track_count{0};
	int16_t division{0};
};


class MidiLoader
{
public:

	/** the entry point for reading a MIDI file */
	bool LoadBuffer(chaos::Buffer<char> const & buffer);

protected:

	/** clean the content thata have parsed */
	void Clean();
	/** the effective method to read the buffer */
	bool DoLoadBuffer(BufferReader & reader);

	/** read a sub chunk of data */
	MidiChunk const ReadChunk(BufferReader & reader);
	/** read the header chunk */
	MidiChunk const ReadHeaderChunk(BufferReader & reader);
	/** read a track chunk */
	MidiChunk const ReadTrackChunk(BufferReader & reader);
	/** initialize the track from data contained in the chunk */
	bool InitializeTrackFromChunk(MidiTrack * track, MidiChunk const & track_chunk);
	/** read a variable length time in stream */
	bool ReadVLTime(BufferReader & reader, uint32_t & result);
	/** convert the header chunk into a header structure */
	bool GetHeaderFromChunk(MidiChunk const & chunk, MidiHeader & result);


protected:

	/** the header */
	MidiHeader header;
	/** the tracks */
	std::vector<std::unique_ptr<MidiTrack>> tracks;
};



MidiChunk const MidiLoader::ReadChunk(BufferReader & reader)
{
	MidiChunk result;
	if (reader.IsEnoughData(8))
	{
		// read the chunk name
		reader.ReadN(result.chunk_name, 4);
		// read the size of the data
		uint32_t data_size;
		reader.Read(data_size);
		data_size = chaos::EndianTools::BigEndianToHost(data_size);
		
		if (reader.IsEnoughData(data_size))
		{
			result.bufsize = data_size;
			result.data    = (char *)reader.GetCurrentPosition();
			reader.Advance(data_size);
		}		
	}
	return result;
}

MidiChunk const MidiLoader::ReadHeaderChunk(BufferReader & reader)
{
	MidiChunk const result = ReadChunk(reader);
	if (result.IsHeaderChunk())
		if (result.bufsize == 6)
			return result;
	return MidiChunk();
}

MidiChunk const MidiLoader::ReadTrackChunk(BufferReader & reader)
{
	MidiChunk const result = ReadChunk(reader);
	if (result.IsTrackChunk())
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
	header = MidiHeader();
	tracks.clear();
}

bool MidiLoader::ReadVLTime(BufferReader & reader, uint32_t & result)
{
	result = 0;
		
	unsigned char tmp = 0;
	uint32_t shift = 0;
	while (!reader.Read(tmp) && (shift > 32 - 7)) // 32 - 7 => too much shift !! some data lost
	{
		result |= ((uint32_t)(tmp & ~0x80));	
		// last byte reached
		if ((tmp & 0x80) == 0)
			return true;
		result <<= 7;
		shift += 7;
	}
	return false;
}

bool MidiLoader::InitializeTrackFromChunk(MidiTrack * track, MidiChunk const & track_chunk)
{
	BufferReader reader(track_chunk);
	while(!reader.IsEOF())
	{
		// read the time of the event
		uint32_t event_time = 0;
		if (!ReadVLTime(reader, event_time))
			return false;

		unsigned char signature = 0;
		reader.Read(signature);

		// misformed event
		if ((signature & 0x80) == 0) 
			return false;

		std::unique_ptr<MidiEvent> new_event;


		if (signature == 0xF0 || signature == 0xF7) // System exclusive event
		{
			new_event = std::move(std::unique_ptr<MidiEvent>(new MidiSystemExclusiveEvent(signature)));		
		}
		else if (signature == 0xFF) // meta event
		{
			new_event = std::move(std::unique_ptr<MidiEvent>(new MidiMetaEvent(signature)));		
		}
		else // standard MIDI event
		{
			new_event = std::move(std::unique_ptr<MidiEvent>(new MidiCommandEvent(signature)));		
		}

		if (new_event == nullptr)
			return false;

		if (!new_event->InitializeEventFromChunk(reader))
			return false;
		track->events.push_back(std::move(new_event));
	}
	return true;
}

bool MidiLoader::DoLoadBuffer(BufferReader & reader)
{
	// get the headers
	MidiChunk header_chunk = ReadHeaderChunk(reader);
	if (header_chunk == nullptr)
		return false;

	if (!GetHeaderFromChunk(header_chunk, header))
		return false;
	// read the tracks
	for (int16_t i = 0 ; i < header.track_count ; ++i)
	{
		MidiChunk track_chunk = ReadTrackChunk(reader);
		if (track_chunk == nullptr)
			return false;

		std::unique_ptr<MidiTrack> new_track(new MidiTrack());
		if (new_track != nullptr)
		{
			if (!InitializeTrackFromChunk(new_track.get(), track_chunk))
				return false;		
			tracks.push_back(std::move(new_track));		
		}	
	}
	return true;
}


bool MidiLoader::GetHeaderFromChunk(MidiChunk const & chunk, MidiHeader & result)
{
	BufferReader reader(chunk);
	reader.Read(result.format);
	reader.Read(result.track_count);
	reader.Read(result.division);
	result.format      = chaos::EndianTools::BigEndianToHost(result.format);
	result.track_count = chaos::EndianTools::BigEndianToHost(result.track_count);
	result.division    = chaos::EndianTools::BigEndianToHost(result.division);
	return result.IsValid();
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
	bool b1 = chaos::EndianTools::IsHostLittleEndian();

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


