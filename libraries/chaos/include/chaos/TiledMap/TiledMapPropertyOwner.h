namespace chaos
{	
	namespace TiledMap
	{

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// Property : base class for some properties
		// ==========================================

			/** types of particle */
		enum class PropertyType : int
		{
			ANY = 0,
			INT = 1,
			FLOAT = 2,
			BOOL = 3,
			STRING = 4,
			COLOR = 5,
			OBJECT = 6
		};

		class Property : public BaseObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** constructor */
			using BaseObject::BaseObject;

			/** returns a pointer on the int property */
			int const* GetPropertyInt() const;
			/** returns a pointer on the float property */
			float const* GetPropertyFloat() const;
			/** returns a pointer on the bool property */
			bool const* GetPropertyBool() const;
			/** returns a pointer on the string property */
			std::string const* GetPropertyString() const;
			/** returns a pointer on the color property */
			glm::vec4 const* GetPropertyColor() const;
			/** returns a pointer on the object property */
			int const* GetPropertyObject() const;

			/** returns whether the property is of type int */
			bool IsPropertyInt() const { return (type == PropertyType::INT); }
			/** returns whether the property is of type float */
			bool IsPropertyFloat() const { return (type == PropertyType::FLOAT); }
			/** returns whether the property is of type bool */
			bool IsPropertyBool() const { return (type == PropertyType::BOOL); }
			/** returns whether the property is of type string */
			bool IsPropertyString() const { return (type == PropertyType::STRING); }
			/** returns whether the property is of type color */
			bool IsPropertyColor() const { return (type == PropertyType::COLOR); }
			/** returns whether the property is of type object */
			bool IsPropertyObject() const { return (type == PropertyType::OBJECT); }

			/** returns the name of the property */
			char const * GetName() const { return name.c_str(); }
			/** returns the property type */
			PropertyType GetPropertyType() const { return type; }

		protected:

			/** the name of the property */
			std::string name;
			/** the type of the property */
			PropertyType type = PropertyType::ANY;
		};

		// ==========================================
		// PropertyTemplate : templated specialization for properties
		// ==========================================

		template<typename T, PropertyType TYPE_ID>
		class PropertyTemplate : public Property
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** the type of the property */
			using property_type = T;

			/** constructor */
			PropertyTemplate(BaseObject * in_owner) :
				Property(in_owner)
			{
				type = TYPE_ID;
			}

			/** returns the value of the property */
			property_type GetValue() { return value; }

		public:

			/** the value of the property */
			property_type value;
		};

		// ==========================================
		// Specialization of properties
		// ==========================================

		using PropertyInt = PropertyTemplate<int, PropertyType::INT>;
		using PropertyFloat = PropertyTemplate<float, PropertyType::FLOAT>;
		using PropertyBool = PropertyTemplate<bool, PropertyType::BOOL>;
		using PropertyString = PropertyTemplate<std::string, PropertyType::STRING>;
		using PropertyColor = PropertyTemplate<glm::vec4, PropertyType::COLOR>;
		using PropertyObject = PropertyTemplate<int, PropertyType::OBJECT>;

		// ==========================================
		// PropertyOwner : some objects that have dynamic properties
		// ==========================================

		class PropertyOwner : public BaseObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** constructor */
			using BaseObject::BaseObject;

			/** find property by name */
			virtual Property const * FindProperty(char const * name, PropertyType type_id = PropertyType::ANY) const;

			/** find property without looking elsewhere than our own table */
			Property const * FindInternalProperty(char const * name, PropertyType type_id) const;

			/** find a property of type int */
			int const * FindPropertyInt(char const * name) const;
			/** find a property of type float */
			float const * FindPropertyFloat(char const * name) const;
			/** find a property of type bool */
			bool const * FindPropertyBool(char const * name) const;
			/** find a property of type string */
			std::string const * FindPropertyString(char const * name) const;
			/** find a property of type color */
			glm::vec4 const* FindPropertyColor(char const* name) const;
			/** find a property of type object */
			int const* FindPropertyObject(char const* name) const;

			/** find a typed property with default value */
			int GetPropertyValueInt(char const * name, int default_value) const;
			/** find a typed property with default value */
			float GetPropertyValueFloat(char const * name, float default_value) const;
			/** find a typed property with default value */
			bool GetPropertyValueBool(char const * name, bool default_value) const;
			/** find a typed property with default value */
			std::string GetPropertyValueString(char const * name, char const * default_value) const;
			/** find a typed property with default value */
			glm::vec4 GetPropertyValueColor(char const* name, glm::vec4 const& default_value) const;
			/** find a typed property with default value */
			int GetPropertyValueObject(char const* name, int default_value) const;

			/** create property */
			PropertyInt* CreatePropertyInt(char const* name, int value);
			/** create property */
			PropertyFloat* CreatePropertyFloat(char const* name, float value);
			/** create property */
			PropertyBool* CreatePropertyBool(char const* name, bool value);
			/** create property */
			PropertyString* CreatePropertyString(char const* name, char const* value);
			/** create property */
			PropertyColor* CreatePropertyColor(char const* name, glm::vec4 const & value);
			/** create property */
			PropertyObject* CreatePropertyObject(char const* name, int value);

			/** check whether an object is of given type */
			virtual bool IsObjectOfType(char const* in_type) const;

		protected:

			/** the method to override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element);

			/** load all properties */
			virtual bool DoLoadProperties(tinyxml2::XMLElement const * element);
			/** returns the container of properties */
			virtual tinyxml2::XMLElement const * GetPropertiesChildNode(tinyxml2::XMLElement const * element) const;

		protected:

			/** the properties of the object */
			std::vector<shared_ptr<Property>> properties;
		};

		// ==========================================
		// TypedObject
		// ==========================================

		class TypedObject : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** override */
			virtual Property const* FindProperty(char const* name, PropertyType type_id) const override;
			/** override */
			virtual bool IsObjectOfType(char const* in_type) const override;

		protected:

			/** constructor */
			using PropertyOwner::PropertyOwner;
			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const* element) override;

		public:

			/** object information */
			std::string type;
		};

#endif

	}; // namespace TiledMap

}; // namespace chaos