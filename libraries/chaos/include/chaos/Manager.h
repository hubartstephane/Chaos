#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/FilePath.h>

namespace chaos
{

  // ==============================================================
  // MANAGER
  // ==============================================================

  class Manager : public ReferencedObject
  {

  public:

    /** initialize the manager from a configuration file */
    virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);

  protected:

    /** utility function to remove an object from a list */
    template<typename T, typename FUNC>
    void DoRemoveObject(size_t index, T & vector, FUNC remove_func)
    {
      // ensure the index is valid
      size_t count = vector.size();
      if (index >= count)
        return;
      // copy the intrusive_ptr to prevent the destruction
      auto object = vector[index];
      // remove the object from the array
      if (index != count - 1)
        vector[index] = vector[count - 1];
      vector.pop_back();
      // callback then let the unreferencement manage the object lifetime
      remove_func(object.get());
    }

    /** detach all elements from a list */
    template<typename T, typename FUNC>
    void RemoveAllObjectsFromList(T & vector, FUNC remove_func)
    {
      while (vector.size() > 0)
      {
        // copy the intrusive_ptr to prevent the destruction
        auto object = vector.back();
        // remove the object from the array
        vector.pop_back();
        // callback then let the unreferencement manage the object lifetime
        remove_func(object.get());
      }
    }

    template<typename T, typename U>
    static size_t FindObjectIndexInVector(T * object, U const & vector)
    {
      assert(object != nullptr);
      size_t count = vector.size();
      for (size_t i = 0; i < count; ++i)
        if (vector[i].get() == object)
          return i;
      return count;
    }
    /** a generic function to find an object in a list by its name */
    template<typename T, typename U>
    static T * FindObjectByName(char const * name, U & objects)
    {
      if (name == nullptr)
        return nullptr;

      size_t count = objects.size();
      for (size_t i = 0; i < count; ++i)
      {
        T * object = objects[i].get();
        if (object == nullptr)
          continue;
        if (strcmp(object->GetName(), name) == 0)
          return object;
      }
      return nullptr;
    }

    /** a generic function to find an object in a list by its path */
    template<typename T, typename U>
    static T * FindObjectByPath(FilePathParam const & in_path, U & objects)
    {
      boost::filesystem::path const & resolved_path = in_path.GetResolvedPath();

      size_t count = objects.size();
      for (size_t i = 0; i < count; ++i)
      {
        T * obj = objects[i].get();
        if (obj == nullptr)
          continue;
        if (obj->GetPath() == resolved_path)
          return obj;
      }
      return nullptr;
    }
  };

}; // namespace chaos
