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
    paused(in_paused) {}

  /** copy constructor */
  Clock(Clock const & other):
    clock_time(0.0),
    time_scale(other.time_scale),
    paused(other.paused) {}

  /** returns the internal time */
  double GetClockTime() const { return clock_time; }

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

protected:

  /** advance the clock */
  void TickClock(double delta_time)
  {
    if (!paused)
      clock_time += time_scale * delta_time;
  }

protected:

  /** the time of the clock */
  double clock_time;
  /** a time scale for the clock */
  double time_scale;
  /** whether the clock is paused or not */
  bool   paused;  
};

  /**
   * ClockManager : an object that help managing multiple clock at the time
   */

class ClockManager : public Clock // the absolute time of the clock manager is given by a clock !!
{

public:

  /** add a clock */
  void AddClock(int clock_id, double in_time_scale = 1.0, bool in_paused = false);
  /** remove a clock */
  void RemoveClock(int clock_id);
  /** remove all clocks */
  void Clean();

  /** Update clocks */
  void TickManager(double delta_time);

  /** gets a clock by id */
  Clock * GetClock(int clock_id);
  /** gets a clock by id */
  Clock const * GetClock(int clock_id) const;

protected:

  /** the clocks */
  std::map<int, Clock> clocks;
};

}; // namespace chaos

