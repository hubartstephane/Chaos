namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class NamedInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** a class that describe an object that can be reference by tag and by name */
	class CHAOS_API NamedInterface
	{
	public:

		/** get the name of the object */
		char const* GetName() const { return name.c_str(); }
		/** get the Tag of the object */
		TagType GetTag() const { return tag; }

		/** change the name of the object */
		void SetName(char const* in_name);
		/** change the tag of the object */
		void SetTag(TagType in_tag) { tag = in_tag; }

		/** change the naming of the object */
		void SetObjectNaming(ObjectRequest request);

	protected:

		/** the name of the object */
		std::string name;
		/** the tag of the object */
		TagType tag = 0;
	};

	/** NamedInterfaceWrapper : this is a wrapper to ba able to use NamedInterface's static methods */
	template<typename T>
	class /*CHAOS_API*/ NamedInterfaceWrapper
	{
	public:

		/** special method to have access to NamedInterface static utility functions */
		char const* GetName() const { return object->GetName(); }
		/** special method to have access to NamedInterface static utility functions */
		TagType GetTag() const { return object->GetTag(); }

	public:

		/** the renderable to render */
		shared_ptr<T> object;
	};

	/** function to serialize into JSON */
	CHAOS_API bool SaveIntoJSON(nlohmann::json& json, NamedInterface const& src);
	/** function to serialize from JSON */
	CHAOS_API bool LoadFromJSON(nlohmann::json const& json, NamedInterface& dst);

#endif

}; // namespace chaos