namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename PIXEL_TYPE>
	class ImagePixelAccessor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename PIXEL_TYPE>
	class ImagePixelAccessor
	{
	public:

		/** the type of pixel concerned */
		using type = PIXEL_TYPE;

		/** constructor */
		ImagePixelAccessor(ImageDescription const& in_description) : description(in_description) {}
		/** copy constructor */
		ImagePixelAccessor(ImagePixelAccessor const& src) = default;

		/** check whether the object is valid (the required PIXEL_TYPE does not correspond to the one for the bitmap) */
		bool IsValid() const
		{
			PixelFormat accessor_format = PixelFormat::GetPixelFormat<type>();
			if (accessor_format != description.pixel_format)
				return false;
			return true;
		}

		/** get the description */
		ImageDescription const& GetImageDescription() const { return description; }

		/** access a pixel */
		type& operator () (int x, int y)
		{
			assert(x >= 0);
			assert(y >= 0);
			assert(x < description.width);
			assert(y < description.height);

			char* base = ((char*)description.data) + y * description.pitch_size;

			return *(((type*)base) + x);
		}

		/** access a pixel */
		type const& operator () (int x, int y) const
		{
			assert(x >= 0);
			assert(y >= 0);
			assert(x < description.width);
			assert(y < description.height);

			char const* base = ((char const*)description.data) + y * description.pitch_size;

			return *(((type const*)base) + x);
		}

	protected:

		/** the description we are working on */
		ImageDescription description;
	};

#endif

}; // namespace chaos