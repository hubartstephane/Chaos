namespace chaos
{
	namespace TiledMap
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// TileParticleFlags
		// ==========================================

		// XXX : see ParticleFlags to avoid flag collisions
		namespace TileParticleFlags
		{
			static constexpr int NEIGHBOUR_LEFT   = (1 << 5);
			static constexpr int NEIGHBOUR_RIGHT  = (1 << 6);
			static constexpr int NEIGHBOUR_TOP    = (1 << 7);
			static constexpr int NEIGHBOUR_BOTTOM = (1 << 8);
		};

		// ==========================================
		// Global
		// ==========================================

		inline glm::vec2 const REVERSE_Y_AXIS = glm::vec2(1.0f, -1.0f);

		// ==========================================
		// Functions
		// ==========================================

		/** split the tile pseudo GID into GID/H-FLIP/V-FLIP */
		CHAOS_API int DecodeTileGID(int pseudo_gid, int* particle_flags = nullptr);
		/** returns the number of hexadecimal character from position */
		CHAOS_API int GetHEXCharacterCount(char const* c);
		/** read an attribute as color */
		CHAOS_API bool ReadAttributeHEX(tinyxml2::XMLElement const* element, char const* attribute_name, unsigned int& result);
		/** read a string as color */
		CHAOS_API bool ReadXMLColor(std::string const& str, glm::vec4& result);
		/** read an attribute as color */
		CHAOS_API bool ReadXMLColor(tinyxml2::XMLElement const* element, char const* attribute_name, glm::vec4& result);

		// ==========================================
		// TiledMap coordinate system
		// ==========================================
		//
		//        OBJECT
		//  +-----------------+
		//  |                 |
		//  |                 |
		// 0,0                |
		//  +----+-------------------------------->
		//  |tile|            |
		//  | 0  |            |
		//  o----+------------+
		//  | \
		//  |  \
		//  |   +-> on object on tile (0, 0) has its bottom left corner here
		//  |       (beware, this point is were Y is max)
		//  |
		//  v
		//

		class CHAOS_API BaseObject : public Object
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** get the path */
			boost::filesystem::path GetOwnerPath() const;

			/** get a parent in the hierarchy by class */
			template<typename T>
			T * GetOwner(bool recursive = true)
			{
				BaseObject * object = this;
				T * result = auto_cast(object);
				while (result == nullptr && object->owner != nullptr && recursive)
				{
					object = object->owner;
					result = auto_cast(object);
				}
				return result;
			}

			/** get a parent in the hierarchy by class */
			template<typename T>
			T const * GetOwner(bool recursive = true) const
			{
				BaseObject const * object = this;
				T const * result = auto_cast(object);
				while (result == nullptr && object->owner != nullptr && recursive)
				{
					object = object->owner;
					result = auto_cast(object);
				}
				return result;
			}

			/** get the manager */
			Manager * GetManager();
			/** get the manager */
			Manager const * GetManager() const;
			/** get the map */
			Map * GetMap();
			/** get the map */
			Map const * GetMap() const;

		protected:

			/** constructor */
			BaseObject(BaseObject * in_owner);

			/** utility function to load objects */
			template<typename T, typename ...PARAMS>
			static bool DoLoadObjectListHelper(tinyxml2::XMLElement const * element, std::vector<T> & result, char const * element_name, char const * container_name, PARAMS...params)
			{
				if (container_name != nullptr) // is there an intermediate node to contain all objects
				{
					element = element->FirstChildElement(container_name);
					if (element == nullptr)
						return true;
				}

				tinyxml2::XMLElement const * e = element->FirstChildElement(element_name);
				for (; e != nullptr; e = e->NextSiblingElement(element_name))
				{
					if constexpr (is_shared_ptr_v<T>)
						DoLoadObjectAndInsertInList<std::remove_reference_t<decltype(*result[0].get())>>(e, result, std::forward<PARAMS>(params)...); // decltype returns a REFERENCE !! this must be removed from function template argument
					else
						DoLoadObjectAndInsertInList<T>(e, result, std::forward<PARAMS>(params)...);
				}
				return true;
			}

			template<typename T, typename ...PARAMS>
			static T * DoLoadObjectAndInsertInList(tinyxml2::XMLElement const * element, std::vector<T> & result_vector, PARAMS...params)
			{
				T result;
				if (result.DoLoad(element))
				{
					result_vector.push_back(std::move(result));
					return &result_vector.back();
				}
				return nullptr;
			}

			template<typename T, typename U, typename ...PARAMS>
			static T* DoLoadObjectAndInsertInList(tinyxml2::XMLElement const* element, std::vector<shared_ptr<U>>& result_vector, PARAMS...params)
			{
				T * result = new T(std::forward<PARAMS>(params)...);
				if (result == nullptr)
					return nullptr;
				if (!result->DoLoad(element))
				{
					delete(result);
					return nullptr;
				}
				result_vector.push_back(result);
				return result;
			}

			/** utility function to load a layer */
			bool DoLoadLayersImpl(tinyxml2::XMLElement const* element, std::vector<shared_ptr<LayerBase>>& result);

		protected:

			/** owning object */
			BaseObject * owner = nullptr;
		};

#endif

	}; // namespace TiledMap

}; // namespace chaos


