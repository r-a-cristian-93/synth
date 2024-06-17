/*
	The output files result.json can be viewed with chrome://tracing/

	reference: https://github.com/zjrubin/Visual_Profiler/tree/master
	reference: https://gist.github.com/TheCherno/31f135eea6ee729ab5f26a6908eb3a5e
 */

#ifndef PROFILER
#define PROFILER

#include <chrono>
#include <fstream>
#include <mutex>
#include <thread>
#include <map>
#include <string>
#include <algorithm>

#ifdef DEBUG_PROFILING
	#define PROFILE_SCOPE(name) \
		ProfileTimer time##__LINE__(name)
	#define PROFILE_FUNCTION() \
		PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
	#define PROFILE_SCOPE(name)
	#define PROFILE_FUNCTION()
#endif

struct ProfileResult {
	std::string name = "Default";
	long long start = 0;
	long long end = 0;
	size_t thread_id = 0;
};


class Profiler {
	std::string m_output_file_name = "results.json";
	size_t m_profile_count = 0;
	std::ofstream m_output_stream;
	std::mutex m_lock;

	Profiler() {
		m_output_stream = std::ofstream(m_output_file_name);
		writeHeader();
	}

	void writeHeader() {
		m_output_stream << "{\"otherData\": {},\"traceEvents\":[";
	}
	void writeFooter() {
		m_output_stream << "]}";
	}

public:
	static Profiler& getInstance() {
		static Profiler instance;
		return instance;
	}

	~Profiler() {
		writeFooter();
	}

	void writeProfile(const ProfileResult& result) {
		std::lock_guard<std::mutex> lock(m_lock);

		if (m_profile_count++ > 0) m_output_stream << ",";

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		m_output_stream << "\n{";
		m_output_stream << "\"cat\":\"function\",";
		m_output_stream << "\"dur\":" << (result.end - result.start) << ',';
		m_output_stream << "\"name\":\"" << name << "\",";
		m_output_stream << "\"ph\":\"X\",";
		m_output_stream << "\"pid\":0,";
		m_output_stream << "\"tid\":" << result.thread_id << ",";
		m_output_stream << "\"ts\":" << result.start;
		m_output_stream << "}";
	}
};

class ProfileTimer {
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> ClockType;

	ClockType m_start_timepoint;
	ProfileResult m_result;
	bool m_stop = false;

public:
	ProfileTimer(const std::string& name)
		:m_result({name, 0, 0})
	{
		start();
	}

	~ProfileTimer() {
		stop();
	}

	void start() {
		static long long last_start_time = 0;
		m_start_timepoint = std::chrono::high_resolution_clock::now();
		m_result.start = std::chrono::time_point_cast<std::chrono::microseconds>(m_start_timepoint).time_since_epoch().count();
		m_result.start += (m_result.start == last_start_time ? 1 : 0);

		last_start_time = m_result.start;
		m_stop = false;
	}

	void stop() {
		if (m_stop) return;

		auto end_timepoint = std::chrono::high_resolution_clock::now();

		m_result.end = std::chrono::time_point_cast<std::chrono::microseconds>(end_timepoint).time_since_epoch().count();
		m_result.thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
		Profiler::getInstance().writeProfile(m_result);

		m_stop = true;
	}
};

#endif
