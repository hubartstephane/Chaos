#include <chaos/Chaos.h>

#include <OVR_CAPI.h>

int main(int argc, char ** argv, char ** env)
{
  ovrResult result = ovr_Initialize(nullptr);
  if (!OVR_FAILURE(result))
  {
    ovrSession      session;
    ovrGraphicsLuid luid;

    result = ovr_Create(&session, &luid);
    if (!OVR_FAILURE(result))
    {
      ovrHmdDesc desc       = ovr_GetHmdDesc(session);
      ovrSizei   resolution = desc.Resolution;

    bool skip = false;
    while (!skip)
    {
      double track_time = ovr_GetTimeInSeconds();
      ovrTrackingState ts = ovr_GetTrackingState(session, track_time, false);

      bool hmd_connected = (ts.StatusFlags & ovrStatus_HmdConnected) != 0;
      if (!hmd_connected)
        hmd_connected = hmd_connected;

      bool position_connected = (ts.StatusFlags & ovrStatus_PositionConnected) != 0;
      if (!position_connected)
        position_connected = position_connected;

      ovrVector3f position;
      ovrQuatf    orientation;
      if (ts.StatusFlags & ovrStatus_OrientationTracked)
        orientation = ts.HeadPose.ThePose.Orientation;
      if (ts.StatusFlags & ovrStatus_PositionTracked)
        position = ts.HeadPose.ThePose.Position;
    }

      ovr_Destroy(session);
    }
    ovr_Shutdown();
  }
  
  chaos::WinTools::PressToContinue();

  return 0;
}


