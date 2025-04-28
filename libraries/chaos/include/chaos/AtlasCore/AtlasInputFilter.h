namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class AtlasInputFilterCondition;
	class AtlasInputFilter;
	class AtlasInputFilterSet;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* AtlasInputFilterCondition : a condition to know whether the bitmap is to be processed
	*/

	class CHAOS_API AtlasInputFilterCondition : public Object, public JSONSerializableInterface
	{
		CHAOS_DECLARE_OBJECT_CLASS(AtlasInputFilterCondition, Object);

	public:

		/** whether the processing is to be done on the bitmap */
		virtual bool AcceptBitmap(class AtlasBitmapInfoInput const* input) const;
		/** whether the processing is to be done on the font */
		virtual bool AcceptFont(class AtlasFontInfoInput const* input) const;

		/** loading method from JSON */
		virtual bool SerializeFromJSON(JSONReadConfiguration config) override;
		/** saving method from JSON */
		virtual bool SerializeIntoJSON(nlohmann::json* json) const override;
	};

	/**
	* AtlasInputFilter : check condition on an entry and start the processor
	*/

	class CHAOS_API AtlasInputFilter
	{

	public:

		/** the condition for handling the image */
		shared_ptr<AtlasInputFilterCondition> condition;
		/** the condition for handling the image */
		shared_ptr<ImageProcessor> processor;
	};

	/** loading method from JSON */
	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasInputFilter& dst);
	/** saving method from JSON */
	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasInputFilter const& src);

	/**
	* AtlasInputFilterSet : a whole set of condition/image processor
	*/

	class CHAOS_API AtlasInputFilterSet
	{
	public:

		/** the filters to apply */
		std::vector<AtlasInputFilter> filters;
	};

	/** loading method from JSON */
	CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, AtlasInputFilterSet& dst);
	/** saving method from JSON */
	CHAOS_API bool DoSaveIntoJSON(nlohmann::json* json, AtlasInputFilterSet const& src);

#endif

}; // namespace chaos