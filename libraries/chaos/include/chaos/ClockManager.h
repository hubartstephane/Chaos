#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{
	/**
	 * ClockEventRepetitionInfo
	 */

	class ClockEventRepetitionInfo
	{
	public:

		/** default constructor */
		ClockEventRepetitionInfo() = default;
		/** intialization constructor */
		ClockEventRepetitionInfo(double in_frequency, int in_repetition_count):
			frequency(in_frequency),
			repetition_count(in_repetition_count){}

		/** return an instance with no repetition */
		static ClockEventRepetitionInfo NoRepetition()
		{
			return ClockEventRepetitionInfo();
		}

		ClockEventRepetitionInfo InfiniteRepetition(double in_frequency)
		{
			assert(in_frequency >= 0.0);
			return ClockEventRepetitionInfo(in_frequency, -1);
		}

		ClockEventRepetitionInfo Repetition(double in_frequency, int in_repetition_count)
		{
			assert(in_frequency >= 0.0);
			assert(in_repetition_count >= 0);
			return ClockEventRepetitionInfo(in_frequency, in_repetition_count);
		}

		/** returns true whether the event is repeated */
		bool IsRepeated() const { return (frequency >= 0.0);}
		/** returns true whether the event is repeated for ever  */
		bool IsRepeatedInfinitly() const { return IsRepeated() && (repetition_count < 0);}

	public:
	
		/** the frequency between repetitions (no repetition if < 0) */
		double frequency{-1.0};
		/** the number of repetitions (0 for no repetition, <0 for unlimited repetition, >0 for well known repetition count) */
		int repetition_count{0};
	};


	/**
	* ClockEventInfo : some information that describes the event
	*/

	class ClockEventInfo : public ClockEventRepetitionInfo
	{
	public:

		/** default constructor */
		ClockEventInfo() = default;
		/** constructor with initialization */
		ClockEventInfo(double in_start_time, double in_duration, ClockEventRepetitionInfo const & in_repetition_info = ClockEventRepetitionInfo()):
			ClockEventRepetitionInfo(in_repetition_info),
			start_time(in_start_time),
			duration(in_duration)
		{
		}
		
		/** creates an instance that describes an event to be ticked once */
		static ClockEventInfo SingleTickEvent(double in_start_time, ClockEventRepetitionInfo const & in_repetition_info = ClockEventRepetitionInfo())
		{
			return ClockEventInfo(in_start_time, 0.0, in_repetition_info);
		}
		/** creates an instance that describes an event to be ticked for a given time */
		static ClockEventInfo RangeEvent(double in_start_time, double in_duration, ClockEventRepetitionInfo const & in_repetition_info = ClockEventRepetitionInfo())
		{
			return ClockEventInfo(in_start_time, in_duration,in_repetition_info);
		}
		/** creates an instance that describes an event to be ticked for a given time */
		static ClockEventInfo ForeverEvent(double in_start_time, ClockEventRepetitionInfo const & in_repetition_info = ClockEventRepetitionInfo())
		{
			return ClockEventInfo(in_start_time, -1.0, in_repetition_info);
		}
		
		/** returns true whether the event is to be ticked only once */
		bool IsSingleTickEvent() const { return (duration == 0.0);}
		/** returns true whether the event is to be ticked for a given range */
		bool IsRangeEvent() const { return (duration > 0.0);}
		/** returns true whether the event is to be ticked for an undermined time*/
		bool IsForeverEvent() const { return (duration < 0.0);}

		/** the maximum time the event can be triggered */
		double GetMaxEventTime() const
		{
			if (IsForeverEvent())
				return std::numeric_limits<float>::max();
			if (IsRepeatedInfinitly())
				return std::numeric_limits<float>::max();
			return start_time + duration + (duration + frequency) * (float)repetition_count;
		}

		/** returns true whether this event will never be triggered */
		bool IsTooLate(double current_time) const
		{
			double max_time = GetMaxEventTime();
			if (current_time > max_time)
				return false;
			return true;
		}

		/** the initial time */
		double start_time{0.0};
		/** duration (0 for single tick event, <0 for unknown duration, >0 for a well known duration) */
		double duration{0.0};
	};

	/**
	* ClockEventTickResult
	*/

	class ClockEventTickResult
	{
	public:

		/** default constructor */
		ClockEventTickResult() {}

	public:
#if 0
		/** the event wants to be restarted later */
		static ClockEventTickResult RestartAt(double time);
		/** the event is finished */
		static ClockEventTickResult Finished();
#endif


	};



	/**
	* Event that can be triggered by clock
	*/

	class ClockEvent : public ReferencedObject
	{
		friend class Clock;

	public:

		/** destructor */
		virtual ~ClockEvent() = default;

		/** duration (0 for single tick event, <0 for unknown duration, >0 for a well known duration) */
		double duration{0.0};
		/** the frequency between repetitions (no repetition if <= 0) */
		double frequency{0.0};
		/** the number of repetitions (0 for no repetition, <0 for unlimited repetition, >0 for well known repetition count) */
		int repetition_count{0};
		/** the method to process */
		virtual bool Process(double clock_time) { return true; }

	protected:

		/** the clock it belongs to */
		class Clock * clock{nullptr};
	};

	/**
	* ClockEventRegistration : the registration of an event in a time line
	*/

	class ClockEventRegistration
	{
	public:

		/** the start time */
		double start_time{0.0};
		/** the ID of the event */
		int    event_id{0};
		/** the event to be played */
		boost::intrusive_ptr<ClockEvent> clock_event;
	};


	/**
	* ClockCreateParams : data for clock creation
	*/

	class ClockCreateParams
	{
	public:

		/** a time scale for the clock */
		double time_scale{1.0};
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

		/** add an event to be ticked. returns its ID */
		int RegisterEvent(ClockEvent * clock_event, double start_time);
		/** Remove an event by pointer (beware, issue using same pointer) */
		bool RemoveEvent(ClockEvent * clock_event);
		/** Remove an event by ID */
		bool RemoveEvent(int event_id);

	protected:

		/** advance the clock */
		bool TickClockImpl(double delta_time, std::vector<ClockEventRegistration> & clock_events);
		/** internal methods to trigger all the event */
		void TriggerClockEvents(std::vector<ClockEventRegistration> & clock_events);
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
		Clock * parent_clock{nullptr};
		/** the time of the clock */
		double clock_time{0.0};
		/** a time scale for the clock */
		double time_scale{1.0};
		/** whether the clock is paused or not */
		bool   paused{false};  
		/** the ID of the clock */
		int    clock_id{0};
		/** the ID for the events */
		int    next_event_id{0};

		/** the events */
		std::vector<ClockEventRegistration> registered_events;
		/** the child clocks */
		std::vector<boost::intrusive_ptr<Clock>> children_clocks;
	};

}; // namespace chaos

