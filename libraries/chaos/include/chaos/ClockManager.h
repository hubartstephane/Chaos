#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{
	/** 
	 * ClockTickData : information for tick
	 */

	class ClockTickData
	{
	public:

		/** the absolute time */
		double clock_time{0.0};
		/** the delta time */
		double delta_time{0.0};
		/** the delta loop count */
		double delta_count{0.0};	
	};


	/**
	* Event that can be triggered by clock
	*/

	class ClockEvent
	{
	public:

		/** destructor */
		virtual ~ClockEvent() = default;



	};

	/**
	 * ClockCreateParams : data for clock creation
	 */

	class ClockCreateParams
	{
	public:

		/** a time scale for the clock */
		double time_scale{1.0};
		/** a frequency at with clock is rest */
		double time_frequency{0.0};
		/** whether the clock is paused or not */
		bool   paused{false};  	
	};

	/**
	* This represents a clock that may paused or have a different time scale than absolute time. It can have inner clocks
	*/

	class Clock : public ReferencedObject
	{

	public:

		/** the constructor */
		Clock(ClockCreateParams const & params = ClockCreateParams());
		/** the destructor */
		virtual ~Clock();

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
		/** change the time frequency */
		void SetTimeFrequency(double new_frequency) { time_frequency = new_frequency; }
		/** returns the time frequency */
		double GetTimeFrequency() const { return time_frequency; }
		/** gets a clock by id */
		Clock * GetChildClock(int id, bool recursive = true);
		/** gets a clock by id */
		Clock const * GetChildClock(int id, bool recursive = true) const;

		/** get the top level clock */
		Clock * GetTopLevelParent();
		/** get the top level clock */
		Clock const * GetTopLevelParent() const;

		/** advance the clock (public interface) */
		bool TickClock(double delta_time);
		/** add a clock */
		Clock * AddChildClock(int id, ClockCreateParams const & params = ClockCreateParams());
		/** remove a clock */
		bool RemoveChildClock(Clock * clock);

	protected:

		/** advance the clock */
		bool TickClockImpl(ClockTickData tick_data);
		/** ensure given clock is a child of the hierarchy tree */
		bool IsDescendantClock(Clock const * clock) const;
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
		/** a frequency at with clock is rest */
		double time_frequency{0.0};
		/** whether the clock is paused or not */
		bool   paused{false};  
		/** the ID of the clock */
		int    clock_id{0};

		/** the child clocks */
		std::vector<boost::intrusive_ptr<Clock>> children_clocks;
	};

}; // namespace chaos

