#include <chaos/HTMLTools.h>
#include <chaos/StringTools.h>

namespace chaos
{
    tinyxml2::XMLElement * HTMLTools::PushElement(tinyxml2::XMLNode * parent, char const * name)
    {
      assert(parent != nullptr);
      assert(name   != nullptr);
      tinyxml2::XMLElement * result = doc->NewElement(name);
      if (result != nullptr)
        InsertNodeAsChild(parent, result);
      return result;
    }

    tinyxml2::XMLText * HTMLTools::PushText(tinyxml2::XMLNode * parent, char const * txt)
    {
      assert(parent != nullptr);
      assert(txt    != nullptr);
      tinyxml2::XMLText * result = doc->NewText(txt);
      if (result != nullptr)
        InsertNodeAsChild(parent, result);
      return result;
    }

    void HTMLTools::InsertNodeAsChild(tinyxml2::XMLNode * parent, tinyxml2::XMLNode * node)
    {
      assert(parent != nullptr);
      assert(node   != nullptr);
      if (parent->FirstChild() == nullptr)
        parent->InsertFirstChild(node);
      else
        parent->InsertAfterChild(parent->LastChild(), node);  
    }

    void HTMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, char const * value)
    {
      assert(element != nullptr);
      assert(name    != nullptr);
      assert(value   != nullptr);
      element->SetAttribute(name, value);
    }

    void HTMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, int value)
    {
      assert(element != nullptr);
      assert(name    != nullptr);
      element->SetAttribute(name, value);
    }

    void HTMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, float value)
    {
      assert(element != nullptr);
      assert(name    != nullptr);
      element->SetAttribute(name, value);
    }

    void HTMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, double value)
    {
      assert(element != nullptr);
      assert(name    != nullptr);
      element->SetAttribute(name, value);
    }

    void HTMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, bool value)
    {
      assert(element != nullptr);
      assert(name    != nullptr);
      element->SetAttribute(name, value);
    }

    void HTMLTools::PushAttribute(tinyxml2::XMLElement * element, char const * name, unsigned int value)
    {
      assert(element != nullptr);
      assert(name    != nullptr);
      element->SetAttribute(name, value);
    }


};

