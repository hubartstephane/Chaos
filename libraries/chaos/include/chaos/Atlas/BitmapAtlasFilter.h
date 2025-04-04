namespace chaos
{
	namespace BitmapAtlas
	{
#ifdef CHAOS_FORWARD_DECLARATION

		class BitmapAtlasFilterCondition;
		class BitmapAtlasFilter;
		class BitmapAtlasFilterSet;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/**
		* BitmapAtlasFilterCondition : a condition to know whether the bitmap is to be processed
		*/

		class CHAOS_API BitmapAtlasFilterCondition : public Object, public JSONSerializableInterface
		{
			CHAOS_DECLARE_OBJECT_CLASS(BitmapAtlasFilterCondition, Object);

		public:

			/** whether the processing is to be done on the bitmap */
			virtual bool AcceptBitmap(class BitmapInfoInput const* input) const;
			/** whether the processing is to be done on the font */
			virtual bool AcceptFont(class FontInfoInput const* input) const;

			/** loading method from JSON */
			virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
			/** saving method from JSON */
			virtual bool SerializeIntoJSON(nlohmann::json * json) const override;
		};

		/**
		* BitmapAtlasFilter : check condition on an entry and start the processor
		*/

		class CHAOS_API BitmapAtlasFilter
		{

		public:

			/** the condition for handling the image */
			shared_ptr<BitmapAtlasFilterCondition> condition;
			/** the condition for handling the image */
			shared_ptr<ImageProcessor> processor;
		};

		/** loading method from JSON */
		CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, BitmapAtlasFilter& dst);
		/** saving method from JSON */
		CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, BitmapAtlasFilter const& src);

		/**
		* BitmapAtlasFilterSet : a whole set of condition/image processor
		*/

		class CHAOS_API BitmapAtlasFilterSet
		{
		public:

			/** the filters to apply */
			std::vector<BitmapAtlasFilter> filters;
		};

		/** loading method from JSON */
		CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, BitmapAtlasFilterSet& dst);
		/** saving method from JSON */
		CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, BitmapAtlasFilterSet const& src);

#endif

	}; // namespace BitmapAtlas

}; // namespace chaos