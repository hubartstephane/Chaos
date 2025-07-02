namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GPURenderContextFrameStats;
	class GPURenderContextStats;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * GPURenderContextFrameStats: statistic for a single frame
	 */

	class GPURenderContextFrameStats
	{
	public:

		uint64_t rendering_timestamp = 0;
		int      drawcall_counter    = 0;
		int      vertices_counter    = 0;
		float    frame_start_time    = 0.0f;
		float    frame_end_time      = 0.0f;
	};

	/**
	 * GPURenderContextStats: statistics for GPURenderContext
	 */

	class CHAOS_API GPURenderContextStats
	{
		friend class GPURenderContext;

	public:

		/** constructor */
		GPURenderContextStats();

		/** get the last delta_time */
		float GetLastDeltaTime() const { return last_delta_time; }
		/** get the average frame rate */
		float GetAverageFrameRate() const;
		/** get the number of average draw calls */
		int GetAverageDrawCalls() const;
		/** get the number of average rendered vertices */
		int GetAverageVertices() const;

		/** get the stats among time */
		boost::circular_buffer<GPURenderContextFrameStats> const & GetFrameStats() const { return frame_stats; }

	protected:

		/** tick method */
		void Tick(float delta_time);
		/** called whenever a new frame is started */
		void OnBeginRenderingFrame(uint64_t rendering_timestamp);
		/** called whenever a new frame is finished */
		void OnEndRenderingFrame();
		/** called for each draw call */
		void OnDrawCall(int vertice_count);


	protected:

		/** the duration of the last tick */
		float last_delta_time = 0.0f;
		/** for counting frame per seconds */
		TimedAccumulator<float> framerate_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<int> drawcall_counter;
		/** for counting drawcall per seconds */
		TimedAccumulator<int> vertices_counter;
		/** the stats over time */
		boost::circular_buffer<GPURenderContextFrameStats> frame_stats;
		/** the stat for current frame */
		GPURenderContextFrameStats current_frame_stat;
	};

#endif

}; // namespace chaos