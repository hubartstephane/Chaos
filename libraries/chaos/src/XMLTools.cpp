#include <chaos/XMLTools.h>
#include <chaos/StringTools.h>

namespace chaos
{
	tinyxml2::XMLElement * XMLTools::PushElement(tinyxml2::XMLNode * parent, char const * name)
	{
		assert(parent != nullptr);
		assert(name   != nullptr);
		tinyxml2::XMLElement * result = doc->NewElement(name);
		if (result != nullptr)
			InsertNodeAsChild(parent, result);
		return result;
	}

	tinyxml2::XMLText * XMLTools::PushText(tinyxml2::XMLNode * parent, char const * txt)
	{
		assert(parent != nullptr);
		assert(txt    != nullptr);
		tinyxml2::XMLText * result = doc->NewText(txt);
		if (result != nullptr)
			InsertNodeAsChild(parent, result);
		return result;
	}

	void XMLTools::InsertNodeAsChild(tinyxml2::XMLNode * parent, tinyxml2::XMLNode * node)
	{
		assert(parent != nullptr);
		assert(node   != nullptr);
		if (parent->FirstChild() == nullptr)
			parent->InsertFirstChild(node);
		else
			parent->InsertAfterChild(parent->LastChild(), node);  
	}

	void XMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, char const * value)
	{
		assert(element != nullptr);
		assert(name    != nullptr);
		assert(value   != nullptr);
		element->SetAttribute(name, value);
	}

	void XMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, int value)
	{
		assert(element != nullptr);
		assert(name    != nullptr);
		element->SetAttribute(name, value);
	}

	void XMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, float value)
	{
		assert(element != nullptr);
		assert(name    != nullptr);
		element->SetAttribute(name, value);
	}

	void XMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, double value)
	{
		assert(element != nullptr);
		assert(name    != nullptr);
		element->SetAttribute(name, value);
	}

	void XMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, bool value)
	{
		assert(element != nullptr);
		assert(name    != nullptr);
		element->SetAttribute(name, value);
	}

	void XMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, unsigned int value)
	{
		assert(element != nullptr);
		assert(name    != nullptr);
		element->SetAttribute(name, value);
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLAttribute const * attribute, int & result)
	{
		result = attribute->IntValue();
		return true;
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLAttribute const * attribute, float & result)
	{
		result = attribute->FloatValue();
		return true;
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLAttribute const * attribute, double & result)
	{
		result = attribute->DoubleValue();
		return true;
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLAttribute const * attribute, bool & result)
	{
		result = attribute->BoolValue();
		return true;
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLAttribute const * attribute, unsigned int & result)
	{
		result = attribute->UnsignedValue();
		return true;
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLAttribute const * attribute, std::string & result)
	{
		if (attribute != nullptr && attribute->Value() != nullptr)
			result = attribute->Value();
		else
			result.clear();
		return true;
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLAttribute const * attribute, boost::filesystem::path & result)
	{
		std::string tmp;
		if (!ReadAttribute(attribute, tmp))
			return false;
		result = tmp;
		return true;
	}


	bool XMLTools::ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, int & result)
	{
		tinyxml2::XMLAttribute const * attribute = element->FindAttribute(attribute_name);
		if (attribute == nullptr)
			return false;
		return ReadAttribute(attribute, result);
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, float & result)
	{
		tinyxml2::XMLAttribute const * attribute = element->FindAttribute(attribute_name);
		if (attribute == nullptr)
			return false;
		return ReadAttribute(attribute, result);
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, double & result)
	{
		tinyxml2::XMLAttribute const * attribute = element->FindAttribute(attribute_name);
		if (attribute == nullptr)
			return false;
		return ReadAttribute(attribute, result);
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, bool & result)
	{
		tinyxml2::XMLAttribute const * attribute = element->FindAttribute(attribute_name);
		if (attribute == nullptr)
			return false;
		return ReadAttribute(attribute, result);
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, unsigned int & result)
	{
		tinyxml2::XMLAttribute const * attribute = element->FindAttribute(attribute_name);
		if (attribute == nullptr)
			return false;
		return ReadAttribute(attribute, result);
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, std::string & result)
	{
		tinyxml2::XMLAttribute const * attribute = element->FindAttribute(attribute_name);
		if (attribute == nullptr)
			return false;
		return ReadAttribute(attribute, result);
	}

	bool XMLTools::ReadAttribute(tinyxml2::XMLElement const * element, char const * attribute_name, boost::filesystem::path & result)
	{
		tinyxml2::XMLAttribute const * attribute = element->FindAttribute(attribute_name);
		if (attribute == nullptr)
			return false;
		return ReadAttribute(attribute, result);
	}

}; // namespace chaos

