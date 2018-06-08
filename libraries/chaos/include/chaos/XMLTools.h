#pragma once

#include <chaos/StandardHeaders.h>

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
    static bool ReadEnumAttribute(tinyxml2::XMLElement const * element, char const * name, std::pair<char const *, int> const * values, int & result_value);

  protected:

    /** insert a child node */
    void InsertNodeAsChild(tinyxml2::XMLNode * parent, tinyxml2::XMLNode * node);

  public:

    /** the tinyxml2 document */
    tinyxml2::XMLDocument * doc;
  };
};
