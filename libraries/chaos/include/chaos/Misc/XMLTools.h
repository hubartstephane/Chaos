#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class XMLTools;

}; // namespace chaos

#else 

namespace chaos
{

	/**
	* HTML is a set of utility function to create HTML document with tinyxml2
	*/
	class XMLTools
	{
	public:

		/** constructor */
		XMLTools(tinyxml2::XMLDocument * in_doc) : doc(in_doc) { assert(doc != nullptr); }

		/** create an element an give it to its parent */
		tinyxml2::XMLElement * PushElement(tinyxml2::XMLNode * parent, char const * name);
		/** create a text an give it to its parent */
		tinyxml2::XMLText * PushText(tinyxml2::XMLNode * parent, char const * txt);

		/** create an attribute for a given element */
		void PushAttribute(tinyxml2::XMLElement * element, char const * name, char const * value);
		/** create an attribute for a given element */
		void PushAttribute(tinyxml2::XMLElement * element, char const * name, int value);
		/** create an attribute for a given element */
		void PushAttribute(tinyxml2::XMLElement * element, char const * name, float value);
		/** create an attribute for a given element */
		void PushAttribute(tinyxml2::XMLElement * element, char const * name, double value);
		/** create an attribute for a given element */
		void PushAttribute(tinyxml2::XMLElement * element, char const * name, bool value);
		/** create an attribute for a given element */
		void PushAttribute(tinyxml2::XMLElement * element, char const * name, unsigned int value);

		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLAttribute const * attribute, int & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLAttribute const * attribute, float & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLAttribute const * attribute, double & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLAttribute const * attribute, bool & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLAttribute const * attribute, unsigned int & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLAttribute const * attribute, std::string & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLAttribute const * attribute, boost::filesystem::path & result);

		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, int & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, float & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, double & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, bool & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, unsigned int & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, std::string & result);
		/** Find an attribute a read into result */
		static bool ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, boost::filesystem::path & result);
		/** Find an attribute, read it as a string an find corresponding value in translation map (returns very last value in case of failure) */
		template<typename T, typename ENCODE_TABLE>
		static bool ReadEnumAttribute(tinyxml2::XMLElement const* element, char const* attribute_name, ENCODE_TABLE const& encode_table, T& dst)
		{	
			// read attribute and convert into enum
			std::string str;
			if (ReadAttribute(element, attribute_name, str))
				return EnumTools::StringToEnum(str.c_str(), encode_table, dst);
			// find the very last entry to get the default value
			size_t i = 0;
			while (i < encode_table.size() && encode_table[i].second != nullptr)
				++i;
			if (i < encode_table.size())
				dst = encode_table[i].first;
			return false;
		}

	protected:

		/** insert a child node */
		void InsertNodeAsChild(tinyxml2::XMLNode * parent, tinyxml2::XMLNode * node);

	public:

		/** the tinyxml2 document */
		tinyxml2::XMLDocument * doc = nullptr;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


