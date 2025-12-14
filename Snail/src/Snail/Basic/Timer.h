#pragma once

#include "Core.h"

#include <mutex>
#include <chrono>
#include <fstream>
#include <algorithm>

namespace Snail {

    class Instrumentor {
    private:
        std::string m_SessionName;
        std::ofstream m_OutputStream;
        int m_ProfileCount;
        std::mutex m_Lock;
        bool m_ActiveSession;
    public:
        ~Instrumentor() {
            EndSession();
        }

        Instrumentor(const Instrumentor&) = delete;

        static void BeginSession(const std::string& name, const std::string& filepath = "Results.json") {
            GetInstance().BeginSessionImpl(name, filepath);
        }

        static void EndSession() {
            GetInstance().EndSessionImpl();
        }

        static void WriteProfile(const ProfileResult& result) {
            GetInstance().WriteProfileImpl(result);
        }

    private:

        Instrumentor()
            : m_SessionName("None"), m_ProfileCount(0), m_ActiveSession(false)
        {
        }

        static Instrumentor& GetInstance() {
            static Instrumentor instance;
            return instance;
        }

        void BeginSessionImpl(const std::string& name, const std::string& filepath = "Results.json") {
            if (m_ActiveSession) {
                EndSession();
            }
            m_ActiveSession = true;
            m_SessionName = name;

            m_OutputStream.open(m_SessionName + std::string("_") + filepath);
            WriteHeader();
        }

        void EndSessionImpl() {
            if (!m_ActiveSession) {
                return;
            }
            m_ActiveSession = false;
            m_ProfileCount = 0;

            WriteFooter();
            m_OutputStream.close();
        }

        void WriteHeader() {
            m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
            m_OutputStream.flush();
        }

        void WriteFooter() {
            m_OutputStream << "]}";
            m_OutputStream.flush();
        }

        void WriteProfileImpl(const ProfileResult& result) {
            std::lock_guard<std::mutex> lockGuard(m_Lock);

            if (m_ProfileCount++ > 0)
                m_OutputStream << ",";

            std::string name = result.name;
            std::replace(name.begin(), name.end(), '"', '\'');

            m_OutputStream << "{";
            m_OutputStream << "\"cat\":\"function\",";
            m_OutputStream << "\"dur\":" << (result.end - result.start) << ',';
            m_OutputStream << "\"name\":\"" << name << "\",";
            m_OutputStream << "\"ph\":\"X\",";
            m_OutputStream << "\"pid\":\"" << m_SessionName << "\",";
            m_OutputStream << "\"tid\":" << result.threadID << ",";
            m_OutputStream << "\"ts\":" << result.start;
            m_OutputStream << "}";

            m_OutputStream.flush();
        }
    };

	class Timer {
	private:
		std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
        std::string m_Name;
	public:
		Timer(const std::string& name)
			: m_Name(name)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~Timer()
		{
			auto EndTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(EndTimepoint).time_since_epoch().count();

			uint64_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Instrumentor::WriteProfile({ m_Name, start, end, threadID });
		}
	};

}
