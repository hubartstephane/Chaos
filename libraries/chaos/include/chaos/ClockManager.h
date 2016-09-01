#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

  /**
   * This represents a clock that may paused or have a different time scale than absolute time
   */

class Clock
{

  friend class ClockManager;

public:

  /** constructor */
  Clock(double in_time_scale = 1.0, bool in_paused = false) :
    clock_time(0.0),
    time_scale(in_time_scale),
    paused(in_paused),
    clock_id(0){}

  /** copy constructor */
  Clock(Clock const & other):
    clock_time(0.0),
    time_scale(other.time_scale),
    paused(other.paused),
    clock_id(other.clock_id) {}

  /** returns the internal time */
  double GetClockTime() const { return clock_time; }
  /** returns the clock ID */
  int GetClockID() const { return clock_id; }

  /** pause the clock */
  void Pause() { paused = true; }
  /** resume the clock */
  void Resume() { paused = false; }
  /** toggle the clock */
  void Toggle() { paused = !paused; }
  /** returns the status of the clock */
  bool IsPaused() const { return paused; }
  /** Reset the time to 0 */
  void Reset(){ clock_time = 0.0; }

  /** change the time scale */
  void SetTimeScale(double new_scale) { time_scale = new_scale; }
  /** returns the time scale */
  double GetTimeScale() const { return time_scale; }

  /** advance the clock */
  bool TickClock(double delta_time)
  {
    if (paused || time_scale == 0.0)
      return false;      
    clock_time += time_scale * delta_time;
    return true;
  }

protected:

  /** the time of the clock */
  double clock_time;
  /** a time scale for the clock */
  double time_scale;
  /** whether the clock is paused or not */
  bool   paused;  
  /** the ID of the clock */
  int    clock_id;
};

  /**
   * ClockManager : an object that help managing multiple clock at the time
   */

class ClockManager : public Clock // the absolute time of the clock manager is given by a clock !!
{

public:

  /** destructor */
  ~ClockManager();

  /** add a clock */
  Clock * AddClock(int clock_id, double in_time_scale = 1.0, bool in_paused = false);
  /** remove a clock */
  bool RemoveClock(int clock_id);
  /** remove all clocks */
  void Clean();

  /** Update clocks */
  bool TickManager(double delta_time);

  /** gets a clock by id */
  Clock * GetClock(int clock_id);
  /** gets a clock by id */
  Clock const * GetClock(int clock_id) const;

protected:

  /** gets a free ID for a clock */
  int FindUnusedID() const;

protected:

  /** the clocks */
  std::vector<Clock *> clocks;
};

}; // namespace chaos

