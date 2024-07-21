namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class MidiChunk;
	class MidiEvent;
	class MidiSystemExclusiveEvent;
	class MidiMetaEvent;
	class MidiCommandEvent;
	class MidiTrack;
	class MidiHeader;
	class MidiLoader;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* MidiChunk : this is a chunk of memory that contains MIDI data
	*/
	class CHAOS_API MidiChunk : public Buffer<char>
	{
	public:

		/** returns true whether the chunk is a header chunk */
		bool IsHeaderChunk() const { return IsTypedTrackChunk("MThd"); }
		/** returns true whether the chunk is a track chunk */
		bool IsTrackChunk() const { return IsTypedTrackChunk("MTrk"); }
		/** utility method to test for chunk name */
		bool IsTypedTrackChunk(char const* type_name) const { return (strncmp(chunk_name, type_name, 4) == 0); }

	public:
		/** the name for the chunk */
		char chunk_name[5] = { 0, 0, 0, 0, 0 };
	};

	/**
	* MidiEvent : an event in MIDI file
	*/

	class CHAOS_API MidiEvent
	{
	public:

		/** constructor */
		MidiEvent(unsigned char in_signature) : signature(in_signature) {}
		/** destructor */
		virtual ~MidiEvent() = default;
		/** try to initialize the event from the chunk of memory */
		virtual bool InitializeEventFromChunk(BufferReader& reader) { return false; }

	public:

		/** the signature of the event */
		unsigned char signature = 0;
	};

	/**
	* MidiSystemExclusiveEvent : an event in MIDI file
	*/

	class CHAOS_API MidiSystemExclusiveEvent : public MidiEvent
	{
	public:

		/** constructor */
		MidiSystemExclusiveEvent(unsigned char in_signature) : MidiEvent(in_signature) {}

		virtual bool InitializeEventFromChunk(BufferReader& reader) override;
	};

	/**
	* MidiMetaEvent : an event in MIDI file
	*/

	class CHAOS_API MidiMetaEvent : public MidiEvent
	{
	public:

		/** constructor */
		MidiMetaEvent(unsigned char in_signature) : MidiEvent(in_signature) {}

		virtual bool InitializeEventFromChunk(BufferReader& reader) override;
	};

	class CHAOS_API MidiCommandEvent : public MidiEvent
	{
	public:

		/** constructor */
		MidiCommandEvent(unsigned char in_signature) : MidiEvent(in_signature) {}

		virtual bool InitializeEventFromChunk(BufferReader& reader) override;
	};

	/**
	* MidiTrack : A track in midi files
	*/

	class CHAOS_API MidiTrack : public NoCopyClass
	{
	public:

		/** destructor */
		virtual ~MidiTrack() = default;

	public:

		/** the events */
		std::vector<std::unique_ptr<MidiEvent>> events;
	};

	/**
	* MidiHeader : The midi file header
	*/

	class CHAOS_API MidiHeader
	{
	public:

		/** returns true whether the header is correct */
		bool IsValid() const;

	public:

		static int16_t const FORMAT_SINGLE_TRACK = 0;
		static int16_t const FORMAT_MULTIPLE_TRACK = 1;
		static int16_t const FORMAT_MULTIPLE_SONG = 2;

		/** the format of the MIDI file */
		int16_t format = 0;
		/** the number of track in MIDI file */
		int16_t track_count = 0;
		/** time description */
		int16_t division = 0;
	};

	/**
	* MidiLoader : the class for reading MIDI file
	*/

	class CHAOS_API MidiLoader : public NoCopyClass
	{
	public:

		/** destructor */
		virtual ~MidiLoader() = default;

		/** the entry point for reading a MIDI file */
		bool LoadBuffer(Buffer<char> const& buffer);

	protected:

		/** clean the content thata have parsed */
		void Clean();
		/** the effective method to read the buffer */
		bool DoLoadBuffer(BufferReader& reader);

		/** read a sub chunk of data */
		MidiChunk const ReadChunk(BufferReader& reader);
		/** read the header chunk */
		MidiChunk const ReadHeaderChunk(BufferReader& reader);
		/** initialize the track from data contained in the chunk */
		bool InitializeTrackFromChunk(MidiTrack* track, MidiChunk const& track_chunk);
		/** read a variable length time in stream */
		bool ReadVLTime(BufferReader& reader, uint32_t& result);
		/** convert the header chunk into a header structure */
		bool GetHeaderFromChunk(MidiChunk const& chunk, MidiHeader& result);

	protected:

		/** the header */
		MidiHeader header;
		/** the tracks */
		std::vector<std::unique_ptr<MidiTrack>> tracks;
	};


#endif

}; // namespace chaos