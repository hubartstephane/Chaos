namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class NamedObjectFilter;
	class NamedObjectFilterList;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** NamedObjectFilter : a class to filter objects by their name */
	class CHAOS_API NamedObjectFilter : public Object
	{
	public:

		virtual bool CheckName(NamedInterface const& object) const { return true; }
	};

	/** NamedObjectFilterList : filter objects with a list */
	class CHAOS_API NamedObjectFilterList : public NamedObjectFilter
	{
	public:

		/** override */
		virtual bool CheckName(NamedInterface const& object) const override;

	public:

		/** if not empty, refuse objects that are not in this list (or in enable_tags) */
		std::vector<std::string> enable_names;
		/** if not empty, refuse objects that are not in this list (or in enable_names) */
		std::vector<TagType> enable_tags;
		/** refuse any object in this list */
		std::vector<std::string> forbidden_names;
		/** refuse any object in this list */
		std::vector<TagType> forbidden_tags;
	};

#endif

}; // namespace chaos