#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	/**
	* This represents a clock that may paused or have a different time scale than absolute time. It can have inner clocks
	*/

	class Clock
	{
    
	public:

		/** returns the internal time */
		double GetClockTime() const { return clock_time; }
		/** returns the clock ID */
		int GetClockID() const { return clock_id; }
		/** returns the parent clock */
		Clock * GetParentClock(){ return parent_clock;}
		/** returns the parent clock */
		Clock const * GetParentClock() const { return parent_clock;}

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
    /** gets a clock by id */
    Clock * GetChildClock(int id, bool recursive = true);
    /** gets a clock by id */
    Clock const * GetChildClock(int id, bool recursive = true) const;

		/** advance the clock */
    bool TickClock(double delta_time);
		/** add a clock */
		Clock * AddChildClock(int id, double in_time_scale = 1.0, bool in_paused = false);
		/** remove a clock */
		bool RemoveChildClock(int id, bool recursive = true);

	protected:

		/** gets a free ID for a clock */
		int FindUnusedID(bool recursive) const;
    /** iterate over the children and get min and max ID's in use */
    void FindUnusedIDStep1(int & smaller_id, int & bigger_id, bool recursive) const;
    /** iterate over the children store all ID's in use */
    void FindUnusedIDStep2(std::vector<int> & IDs, bool recursive) const;

	protected:

		/** the parent clock */
		Clock * parent_clock;
		/** the time of the clock */
		double clock_time{0.0};
		/** a time scale for the clock */
		double time_scale{1.0};
		/** whether the clock is paused or not */
		bool   paused{false};  
		/** the ID of the clock */
		int    clock_id{0};

		/** the child clocks */
		std::vector<std::unique_ptr<Clock>> children_clocks;
	};

}; // namespace chaos

