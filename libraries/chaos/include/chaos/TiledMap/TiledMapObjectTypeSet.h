namespace chaos
{
	namespace TiledMap
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// ObjectTypeDefinition
		// ==========================================

		class CHAOS_API ObjectTypeDefinition : public PropertyOwner
		{

			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** constructor */
			using PropertyOwner::PropertyOwner;

		protected:

			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** override */
			virtual tinyxml2::XMLElement const * GetPropertiesChildNode(tinyxml2::XMLElement const * element) const override
			{
				return element; // XXX: the properties are not contained by a 'properties' node
			}

		public:

			/** object information */
			std::string name;
			/** the color in the editor */
			glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		};

		// ==========================================
		// ObjectTypeSet
		// ==========================================

		class CHAOS_API ObjectTypeSet : public ManagerObject
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** the constructor */
			using ManagerObject::ManagerObject;

			/** find an object type */
			ObjectTypeDefinition * FindObjectType(char const * name);
			/** find an object type */
			ObjectTypeDefinition const * FindObjectType(char const * name) const;

			/** find the property in an ObjectType */
			Property const * FindObjectProperty(char const * type, char const * name, PropertyType type_id = PropertyType::ANY) const;


			/** returns the number of object type */
			size_t GetObjectTypeDefinitionCount() const { return object_types.size(); }
			/** returns the object type by its index */
			ObjectTypeDefinition * GetObjectTypeDefinition(size_t index) { return object_types[index].get(); }
			/** returns the object type by its index */
			ObjectTypeDefinition const * GetObjectTypeDefinition(size_t index) const { return object_types[index].get(); }

		protected:

			/** override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** override */
			virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) override;
			/** override */
			virtual char const * GetXMLMarkupName() const override { return "objecttypes"; }

			/** load all types */
			bool DoLoadObjectTypes(tinyxml2::XMLElement const * element);

		protected:

			/** object set information */
			std::string name;
			/** all types of object */
			std::vector<shared_ptr<ObjectTypeDefinition>> object_types;
		};

#endif

	}; // namespace TiledMap

}; // namespace chaos
