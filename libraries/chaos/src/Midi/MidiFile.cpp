#include <chaos/Chaos.h>

namespace chaos
{
	bool MidiHeader::IsValid() const
	{
		if ((format != FORMAT_SINGLE_TRACK) && (format != FORMAT_MULTIPLE_TRACK) && (format != FORMAT_MULTIPLE_SONG))
			return false;
		if (format == FORMAT_SINGLE_TRACK && track_count != 1)
			return false;
		return true;
	}

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

	MidiChunk const MidiLoader::ReadChunk(BufferReader & reader)
	{
		MidiChunk result;
		if (reader.IsEnoughData(8)) // 8 = name + size
		{
			// read the chunk name
			reader.ReadN(result.chunk_name, 4);
			// read the size of the data
			uint32_t data_size;
			reader.Read(data_size);
			data_size = EndianTools::BigEndianToHost(data_size);

			if (reader.IsEnoughData(data_size))
			{
				result.bufsize = data_size;
				result.data = (char *)reader.GetCurrentPosition();
				reader.Advance(data_size);
			}
		}
		return result;
	}

	MidiChunk const MidiLoader::ReadHeaderChunk(BufferReader & reader)
	{
		MidiChunk const result = ReadChunk(reader);
		if (result.IsHeaderChunk())
			if (result.bufsize == 6) // because header is always 6 bytes long
				return result;
		return MidiChunk();
	}

	bool MidiLoader::LoadBuffer(Buffer<char> const & buffer)
	{
		Clean();
		BufferReader reader(buffer);
		if (!DoLoadBuffer(reader))
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
		uint32_t count = 0;
		while (!reader.Read(tmp) && count++ < 4) // time is at much 4 bytes long
		{
			result |= ((uint32_t)(tmp & ~0x80));		
			// last byte reached
			if ((tmp & 0x80) == 0)
				return true;
			result <<= 7;
		}
		return false;
	}

	bool MidiLoader::InitializeTrackFromChunk(MidiTrack * track, MidiChunk const & track_chunk)
	{
		BufferReader reader(track_chunk);
		while (!reader.IsEOF())
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
		size_t track_found = 0;

		MidiChunk data_chunk = ReadChunk(reader);
		while (data_chunk.data != nullptr)
		{
			// don't know how to handle NON-TRACK chunk
			if (!data_chunk.IsTrackChunk())
				continue;
			// do we already have read all expected track chunk
			if (track_found == header.track_count)
				continue;

			// create a track from the chunk
			std::unique_ptr<MidiTrack> new_track(new MidiTrack());
			if (new_track != nullptr)
			{
				if (!InitializeTrackFromChunk(new_track.get(), data_chunk))
					return false;
				tracks.push_back(std::move(new_track));
			}

			// update counts
			++track_found;
		}

		return (track_found == header.track_count); // all expected tracks read
	}

	bool MidiLoader::GetHeaderFromChunk(MidiChunk const & chunk, MidiHeader & result)
	{
		BufferReader reader(chunk);
		reader.Read(result.format);
		reader.Read(result.track_count);
		reader.Read(result.division);
		result.format = EndianTools::BigEndianToHost(result.format);
		result.track_count = EndianTools::BigEndianToHost(result.track_count);
		result.division = EndianTools::BigEndianToHost(result.division);
		return result.IsValid();
	}
}; // namespace chaos
