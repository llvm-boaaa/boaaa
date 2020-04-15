#ifndef BOAAA_LV_TIMEPASS_H
#define BOAAA_LV_TIMEPASS_H

//define LLVMFunction
#include "include_versions/LLVM_Function.inc"
//define LLVMModule
#include "include_versions/LLVM_Module.inc"
//include llvm/Pass.h
#include "include_versions/LLVM_include_Pass_h.inc"
//define LLVMModulePass
#include "include_versions/LLVM_ModulePass.inc"
//define LLVMFunctionPass
#include "include_versions/LLVM_FunctionPass.inc"
//define LLVMImmutablePass
#include "include_versions/LLVM_ImmutablePass.inc"

#include "boaaa/support/select_type.h"
#include "boaaa/support/data_store.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace boaaa
{
	namespace detail {

		class TimeMessure {
		public:
			using timestamp = typename std::chrono::time_point<std::chrono::high_resolution_clock>;
		private:
			uint16_t m_nanos		= 0;
			uint16_t m_micros		= 0;
			uint16_t m_millis		= 0;
			uint64_t m_seconds		= 0;

			timestamp m_start;
			timestamp m_end;

			bool notset_start = true;
			bool notset_end   = true;

			std::mutex m_write_mutex;

		public:
			timestamp start() {
				return std::chrono::high_resolution_clock::now();
			}

			void addTime(timestamp start) {
				timestamp end = std::chrono::high_resolution_clock::now();

				std::lock_guard<std::mutex> guard(m_write_mutex);

				if (notset_start || m_start > start) {
					m_start = start;
					notset_start = false;
				}

				if (notset_end || m_end < end) {
					m_end = end;
					notset_end = false;
				}

				m_nanos += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() % 1000;
				if (m_nanos >= 1000) {
					m_nanos -= 1000U;
					m_micros++;
				}
				m_micros += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() % 1000;
				if (m_micros >= 1000) {
					m_micros -= 1000U;
					m_millis++;
				}

				m_millis += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() % 1000;
				if (m_millis >= 1000) {
					m_millis -= 1000U;
					m_seconds++;
				}

				m_seconds += std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
			}

			void reset() {
				m_nanos = 0;
				m_micros = 0;
				m_millis = 0;
				m_seconds = 0;

				notset_start = true;
				notset_end = true;
			}

			virtual void printResult(std::ostream& stream) {
				uint16_t  sum_nanos   = std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start).count()  % 1000;
				uint16_t  sum_micros  = std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count() % 1000;
				uint16_t  sum_millis  = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count() % 1000;
				uint64_t sum_seconds = std::chrono::duration_cast<std::chrono::seconds>(m_end - m_start).count();
				
				stream << "Total PM Time : " << sum_seconds << "," << (sum_millis < 100 ? "0" : "") << +(sum_millis < 10 ? "0" : "") << (int) sum_millis
															<< "." << (sum_micros < 100 ? "0" : "") << +(sum_micros < 10 ? "0" : "") << (int) sum_micros
															<< "." << (sum_nanos  < 100 ? "0" : "") << +(sum_nanos  < 10 ? "0" : "") << (int) sum_nanos << "\n";

				stream << "Funtion Time  : " << m_seconds << "," << (m_millis < 100 ? "0" : "") << +(m_millis < 10 ? "0" : "") << (int) m_millis
														  << "." << (m_micros < 100 ? "0" : "") << +(m_micros < 10 ? "0" : "") << (int) m_micros
														  << "." << (m_nanos  < 100 ? "0" : "") << +(m_nanos  < 10 ? "0" : "") << (int) m_nanos << "\n";
			}
		};

		template<class PASS>
		class TimeModulePass : public PASS, TimeMessure {
		public:
			TimeModulePass() : PASS(), TimeMessure() {

			}

			bool runOnModule(LLVMModule& M) override {
				bool result;
				timestamp start_ = start();
				result = PASS::runOnModule(M);
				addTime(start_);
				return result;
			}

			void printResult(std::ostream& stream) override {
				stream << "Report for " << typeid(PASS).name() << ":\n";
				TimeMessure::printResult(stream);
			}
		};

		template<class PASS>
		class TimeFunctionPass : public PASS, TimeMessure {
		private:
			typedef boaaa::data_store<const char*, int> hashobj;
			
			std::vector<uint64_t> functionhashes;

		public:
			TimeFunctionPass() : PASS(), TimeMessure() { }

			bool runOnFunction(LLVMFunction& F) override {
				hashobj hash(F.getName().str().c_str(), F.getNumOperands());
				functionhashes.push_back(hash.hash());
				bool result;
				timestamp start_ = start();
				result = PASS::runOnFunction(F);
				addTime(start_);
				return result;
			}

			void printResult(std::ostream& stream) override {
				stream << "Report for " << typeid(PASS).name() << ":\n";
				TimeMessure::printResult(stream);
			}

			std::vector<uint64_t>& getFunctionHashes() {
				return functionhashes;
			}

		};

		template<class PASS>
		class TimeImmutablePass : public PASS, TimeMessure {

		public:
			TimeImmutablePass() : PASS(), TimeMessure() { }

			//messure no time, because no work is done.
			bool runOnModule(LLVMModule& M) override {
				return PASS::runOnModule(M);
			}

			void printResult(std::ostream& stream) override {
				stream << "TimePass Report for " << typeid(PASS).name() << ":\n";
				stream << "Total PM Time : --\n";
				stream << "Funtion Time  : --\n";
			}

		};
		
		template<class PASS>
		struct select_time_pass {
			using type = typename select_type_reverse_for<std::is_base_of, PASS,
				LLVMImmutablePass, detail::TimeImmutablePass<PASS>,
				LLVMModulePass, detail::TimeModulePass<PASS>,
				LLVMFunctionPass, detail::TimeFunctionPass<PASS>>::type;
		};

		template<class PASS>
		using select_time_pass_t = typename select_time_pass<PASS>::type;
	}

	template<class PASS>
	struct TimePass : public detail::select_time_pass_t<PASS> {
		using super	= PASS;
		using timepass = typename detail::select_time_pass<PASS>::type;

	public:
		TimePass() : timepass() { }

		~TimePass() { }
	};


}

#endif //!BOAAA_LV_TIMEPASS_H