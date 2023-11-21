namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class PersistentDataInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* PersistentDataInterface : a base class to store data in persistent storage
	*/
	class CHAOS_API PersistentDataInterface
	{

	public:

		/** destructor */
		virtual ~PersistentDataInterface() = default;

		/** gets a pointer to the json node where data are stored */
		virtual nlohmann::json * GetPersistentWriteStorage() const { return nullptr; }
		/** gets a pointer to the json node where data are stored */
		virtual nlohmann::json const * GetPersistentReadStorage() const { return nullptr; }

		/** entry point to read data from the storage */
		bool ReadPersistentData();
		/** entry point to store data into the storage */
		bool WritePersistentData() const;

	protected:

		/** read information from persistent storage */
		virtual void OnReadPersistentData(nlohmann::json const * json) {}
		/** write information into persistent storage */
		virtual void OnWritePersistentData(nlohmann::json * json) const {}
	};

#endif

}; // namespace chaos