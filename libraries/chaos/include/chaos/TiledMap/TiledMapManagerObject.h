#ifdef CHAOS_FORWARD_DECLARATION



#else

namespace chaos
{	
	namespace TiledMap
	{
		// ==========================================
		// ManagerObject : objects control by the manager (Map & TileSet)
		// ==========================================

		class ManagerObject : public PropertyOwner
		{
			CHAOS_TILEDMAP_ALL_FRIENDS

		public:

			/** returns true whether the name match the resource name */
			bool IsMatchingName(boost::filesystem::path const & in_path) const;
			/** get the path */
			boost::filesystem::path const & GetPath() const { return path; }

		protected:

			/** the constructor */
			ManagerObject(BaseObject * in_owner, boost::filesystem::path in_path);

			/** loading method from XML */
			virtual bool DoLoadDocument(tinyxml2::XMLDocument const * doc);
			/** the method to override */
			virtual bool DoLoad(tinyxml2::XMLElement const * element) override;
			/** the method to override */
			virtual bool DoLoadMembers(tinyxml2::XMLElement const * element) { return true; }
			/** get the name of the expected markup */
			virtual char const * GetXMLMarkupName() const { return nullptr; }

		public:

			/** the filename */
			boost::filesystem::path path;
		};

	}; // namespace TiledMap

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
