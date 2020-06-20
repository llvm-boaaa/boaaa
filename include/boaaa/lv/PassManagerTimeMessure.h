#ifndef BOAAA_PASS_MANAGER_TIME_MESSURE_H
#define BOAAA_PASS_MANAGER_TIME_MESSURE_H

#include "boaaa/EvaluationResult.h"

namespace boaaa {

	struct PassManagerTimeMessure {
	public:
		using timestamp = typename std::chrono::time_point<std::chrono::high_resolution_clock>;

		PassManagerTimeMessure() : m_start(), m_end() {

		}

		void start() {
			m_start = std::chrono::high_resolution_clock::now();
		}

		void end() {
			m_end = std::chrono::high_resolution_clock::now();
		}

		void printToEvalRes(EvaluationResult& er) {
			uint16_t  sum_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start).count() % 1000;
			uint16_t  sum_micros = std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count() % 1000;
			uint16_t  sum_millis = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count() % 1000;
			uint64_t sum_seconds = std::chrono::duration_cast<std::chrono::seconds>(m_end - m_start).count();

			er.set_pm_time_seconds(sum_seconds);
			er.set_pm_time_millis(sum_millis);
			er.set_pm_time_micros(sum_micros);
			er.set_pm_time_nanos(sum_nanos);
		}

		void printResult(std::ostream& stream) {
			uint16_t  sum_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start).count() % 1000;
			uint16_t  sum_micros = std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count() % 1000;
			uint16_t  sum_millis = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count() % 1000;
			uint64_t sum_seconds = std::chrono::duration_cast<std::chrono::seconds>(m_end - m_start).count();

			stream << "Total PM Time : " << sum_seconds << "," << (sum_millis < 100 ? "0" : "") << +(sum_millis < 10 ? "0" : "") << (int)sum_millis
				<< "." << (sum_micros < 100 ? "0" : "") << +(sum_micros < 10 ? "0" : "") << (int)sum_micros
				<< "." << (sum_nanos < 100 ? "0" : "") << +(sum_nanos < 10 ? "0" : "") << (int)sum_nanos << "\n";
		}

	private:
		timestamp m_start;
		timestamp m_end;
	};

}

#endif