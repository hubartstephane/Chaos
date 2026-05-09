namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class FIBITMAPDeleter;
	class FIMULTIBITMAPDeleter;

	using bitmap_ptr = std::unique_ptr<FIBITMAP, FIBITMAPDeleter>;
	using multibitmap_ptr = std::unique_ptr<FIMULTIBITMAP, FIMULTIBITMAPDeleter>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* FIBITMAPDeleter : deleter for FIBITMAP
	*/

	class CHAOS_API FIBITMAPDeleter
	{
	public:

		/** constructor */
		FIBITMAPDeleter(bool in_release = true);
		/** the destruction method */
		void operator ()(FIBITMAP* bitmap);

	protected:

		/** whether the resource is to be destroyed or not */
		bool release = true;
	};

	/**
	* FIBITMAPDeleter : deleter for FIBITMAP
	*/

	class CHAOS_API FIMULTIBITMAPDeleter
	{
	public:

		/** constructor */
		FIMULTIBITMAPDeleter(bool in_release = true) ;
		/** the destruction method */
		void operator ()(FIMULTIBITMAP* multi_bitmap);

	protected:

		/** whether the resource is to be destroyed or not */
		bool release = true;
	};

#endif

}; // namespace chaos