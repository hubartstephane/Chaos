#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class NamedObjectFilter;
	class NamedObjectFilterList;

}; // namespace chaos

#else 

namespace chaos
{

	/** NamedObjectFilter : a class to filter objects by their name */
	class NamedObjectFilter : public Object
	{
	public:

		virtual bool CheckName(NamedObject const & object) const { return true; }
	};

	/** NamedObjectFilterList : filter objects with a list */
	class NamedObjectFilterList : public NamedObjectFilter
	{
	public:

		/** override */
		virtual bool CheckName(NamedObject const & object) const override;

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

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION