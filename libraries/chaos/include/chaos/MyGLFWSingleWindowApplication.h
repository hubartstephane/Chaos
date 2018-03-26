#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/MyGLFWWindow.h>
#include <chaos/Application.h>
#include <chaos/ClockManager.h>
#include <chaos/SoundManager.h>
#include <chaos/GPUResourceManager.h>

namespace chaos
{
  namespace MyGLFW
  {

    /**
    * SingleWindowApplicationParams : parameters for playing single window application
    */

    class SingleWindowApplicationParams
    {
    public:

      /** the title */
      char const * title = nullptr;
      /** the wanted monitor */
      GLFWmonitor * monitor = nullptr;
      /** the monitor index */
      int monitor_index = 0;
      /** window width */
      int width = 0;
      /** window height */
      int height = 0;
      /** the hints */
      WindowHints hints;
    };

    /**
    * SingleWindowApplication
    */

    class SingleWindowApplication : public Application
    {

    public:

      /** constructor */
      SingleWindowApplication(SingleWindowApplicationParams const & in_window_params);

      /** getter of the singleton instance */
      static inline SingleWindowApplication * GetGLFWApplicationInstance() { return dynamic_cast<SingleWindowApplication*>(singleton_instance); }
      /** getter of the singleton instance */
      static inline SingleWindowApplication const * GetGLFWApplicationConstInstance() { return dynamic_cast<SingleWindowApplication const*>(singleton_instance); }

      /** gets the main clock */
      Clock * GetMainClock() { return main_clock.get(); }
      /** gets the main clock */
      Clock const * GetMainClock() const { return main_clock.get(); }

      /** gets the sound manager */
      SoundManager * GetSoundManager() { return sound_manager.get(); }
      /** gets the sound manager */
      SoundManager const * GetSoundManager() const { return sound_manager.get(); }

      /** gets the graphic resource manager */
      GPUResourceManager * GetGPUResourceManager() { return gpu_manager.get(); }
      /** gets the graphic resource manager */
      GPUResourceManager const * GetGPUResourceManager() const { return gpu_manager.get(); }

    protected:

      /** Main method */
      virtual bool Main() override;

      /** Window Loop */
      bool MessageLoop();

      /** an error callback */
      static void OnGLFWError(int code, const char* msg);
      /** a debugging function to output some message from FreeImage */
      static void FreeImageOutputMessageFunc(FREE_IMAGE_FORMAT fif, const char *msg);
      /** initializing standard libraries */
      virtual bool InitializeStandardLibraries() override;
      /** Finalizalizing standard libraries */
      virtual bool FinalizeStandardLibraries() override;
      /** initialize the application */
      virtual bool Initialize() override;
      /** finalize the application */
      virtual bool Finalize() override;
      /** initialize the managers */
      virtual bool InitializeManagers() override;
      /** finalize the managers */
      virtual bool FinalizeManagers() override;

      /** the method to override for window generation */
      virtual Window * GenerateWindow();
      /** tick all the managers */
      virtual void TickManagers(double delta_time);

      /** Tweak window hints from configuration */
      void TweakHintsFromConfiguration(SingleWindowApplicationParams & params, nlohmann::json const & in_config);

    protected:

      /** the main clock of the manager */
      boost::intrusive_ptr<Clock> main_clock;
      /** the sound manager */
      boost::intrusive_ptr<SoundManager> sound_manager;
      /** the graphic resource manager */
      boost::intrusive_ptr<GPUResourceManager> gpu_manager;

      /** the initial_window param */
      SingleWindowApplicationParams window_params;
      /** the window created */
      Window * window = nullptr;
    };

    /**
    * RunWindowApplication : utility template function to run an application only from a class
    */

    template<typename WINDOW_TYPE>
    bool RunWindowApplication(int argc, char ** argv, char ** env, SingleWindowApplicationParams const & in_window_params)
    {
      class MyApplication : public SingleWindowApplication
      {
      public:
        MyApplication(SingleWindowApplicationParams const & in_window_params) :
          SingleWindowApplication(in_window_params) {}
      protected:
        Window * GenerateWindow() override { return new WINDOW_TYPE; }
      };

      bool result = false;

      MyApplication * application = new MyApplication(in_window_params);
      if (application != nullptr)
      {
        result = application->Run(argc, argv, env);
        delete(application);
      }
      return result;
    }

  }; // namespace MyGLFW

}; // namespace chaos

