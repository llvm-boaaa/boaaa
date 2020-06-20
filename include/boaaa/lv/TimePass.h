#ifndef BOAAA_LV_TIMEPASS_H
#define BOAAA_LV_TIMEPASS_H

//shouldn't be needed to include, but without the code wouldn't compile
#include "llvm/IR/LegacyPassManager.h"

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
//define LLVMLegacyPassManager
#include "include_versions/LLVM_LegacyPassManager.inc"

#include "boaaa/EvaluationResult.h"
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

			timestamp m_start		= timestamp();
			timestamp m_end			= timestamp();

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
				
				stream << "Analysis Time : " << sum_seconds << "," << (sum_millis < 100 ? "0" : "") << +(sum_millis < 10 ? "0" : "") << (int) sum_millis
															<< "." << (sum_micros < 100 ? "0" : "") << +(sum_micros < 10 ? "0" : "") << (int) sum_micros
															<< "." << (sum_nanos  < 100 ? "0" : "") << +(sum_nanos  < 10 ? "0" : "") << (int) sum_nanos << "\n";

				stream << "Funtion Time  : " << m_seconds << "," << (m_millis < 100 ? "0" : "") << +(m_millis < 10 ? "0" : "") << (int) m_millis
														  << "." << (m_micros < 100 ? "0" : "") << +(m_micros < 10 ? "0" : "") << (int) m_micros
														  << "." << (m_nanos  < 100 ? "0" : "") << +(m_nanos  < 10 ? "0" : "") << (int) m_nanos << "\n";
			}

			virtual void printToEvalRes(EvaluationResult& er) {
				uint16_t  sum_nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(m_end - m_start).count() % 1000;
				uint16_t  sum_micros = std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start).count() % 1000;
				uint16_t  sum_millis = std::chrono::duration_cast<std::chrono::milliseconds>(m_end - m_start).count() % 1000;
				uint64_t sum_seconds = std::chrono::duration_cast<std::chrono::seconds>(m_end - m_start).count();

				er.set_analysis_time_seconds(sum_seconds);
				er.set_analysis_time_millis(sum_millis);
				er.set_analysis_time_micros(sum_micros);
				er.set_analysis_time_nanos(sum_nanos);


				er.set_function_time_seconds(m_seconds);
				er.set_function_time_millis(m_millis);
				er.set_function_time_micros(m_micros);
				er.set_function_time_nanos(m_nanos);
			}

			void concat(TimeMessure* tm) 
			{

				tm->unifyTime();

				if (!tm->notset_start && (notset_start || m_start > tm->m_start)) {
					m_start = tm->m_start;
					notset_start = false;
				}

				if (!tm->notset_end && (notset_end || m_end < tm->m_end)) {
					m_end = tm->m_end;
					notset_end = false;
				}

				m_nanos += tm->m_nanos;
				if (m_nanos >= 1000) {
					m_nanos -= 1000U;
					m_micros++;
				}
				m_micros += tm->m_micros;
				if (m_micros >= 1000) {
					m_micros -= 1000U;
					m_millis++;
				}

				m_millis += tm->m_millis;
				if (m_millis >= 1000) {
					m_millis -= 1000U;
					m_seconds++;
				}

				m_seconds += tm->m_seconds;
			}

			virtual void unifyTime() {};
		};

		template<class PASS>
		class TimeModulePass : public PASS, public TimeMessure {
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

			void printToEvalRes(EvaluationResult& er) override {
				// +6 because typeid starts witch "class classname"
				std::string s = typeid(PASS).name() + 6;
				er.set_aa_name(boaaa::copyString(s.c_str(), s.size()));
				er.set_aa_name_delete(boaaa::delete_mem);
				TimeMessure::printToEvalRes(er);
			}

			void unifyTime() override {};
		};

		template<class PASS>
		class TimeFunctionPass : public PASS, public TimeMessure {
		private:
			typedef boaaa::data_store<const char*, int> hashobj;

		public:
			TimeFunctionPass() : PASS(), TimeMessure() { }

			bool runOnFunction(LLVMFunction& F) override {
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

			void printToEvalRes(EvaluationResult& er) override {
				// +6 because typeid starts witch "class classname"
				std::string s = typeid(PASS).name() + 6;
				er.set_aa_name(boaaa::copyString(s.c_str(), s.size()));
				er.set_aa_name_delete(boaaa::delete_mem);
				TimeMessure::printToEvalRes(er);
			}

			void unifyTime() override {};
		};

		template<class PASS>
		class TimeImmutablePass : public PASS, public TimeMessure {

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

			void printToEvalRes(EvaluationResult& er) override {
				// +6 because typeid starts witch "class classname"
				std::string s = typeid(PASS).name() + 6;
				er.set_aa_name(boaaa::copyString(s.c_str(), s.size()));
				er.set_aa_name_delete(boaaa::delete_mem);
				TimeMessure::printToEvalRes(er);
			}

			void unifyTime() override {};
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
	private:
		using super	= PASS;
		using timepass = typename detail::select_time_pass<PASS>::type;
	public:
		TimePass() : timepass() { }

		~TimePass() { }

		void addPass(LLVMLegacyPassManager& pm) {
			pm.add(this);
		}

		void unifyTime() override {};
	};

	namespace {

		template<size_t N, class ...Passes>
		struct recursiveCaller;

		template<size_t N, class ...Passes>
		struct recursiveCaller 
		{
		private:
			using store = _data_store<Passes...>;
		public:
			using type = typename get_helper<N, store>::type;
			typedef boaaa::data_store<TimePass<Passes>*...> data_store;

			static void addPasses(LLVMLegacyPassManager& pm, data_store& passes)
			{
				recursiveCaller<N - 1, Passes...>::addPasses(pm, passes);
				TimePass<type>* pass = passes.get<N>();
				pass->addPass(pm);
			}

			static std::string unifyNames()
			{
				std::string out = std::string(typeid(type).name()).substr(6) + "->";
				out += recursiveCaller<N - 1, Passes...>::unifyNames();
				return out;
			}

			static void unifyTime(detail::TimeMessure& tm, data_store& passes)
			{
				tm.concat(static_cast<detail::TimeMessure*>(
							static_cast<detail::select_time_pass_t<type>*>(
								static_cast<TimePass<type>*>(passes.get<N>()))));
				recursiveCaller<N - 1, Passes...>::unifyTime(tm, passes);
			}
		};

		template<class ...Passes>
		struct recursiveCaller<0, Passes...> 
		{
		private:
			using store = _data_store<Passes...>;
		public:
			using type = typename get_helper<0, store>::type;
			typedef boaaa::data_store<TimePass<Passes>*...> data_store;

			static void addPasses(LLVMLegacyPassManager& pm, data_store& passes)
			{
				TimePass<type>* pass = passes.get<0>();
				pass->addPass(pm);
			}

			static std::string unifyNames()
			{
				return std::string(typeid(type).name()).substr(6);
			}

			static void unifyTime(detail::TimeMessure& tm, data_store& passes)
			{
				tm.concat(static_cast<detail::TimeMessure*>(
							static_cast<detail::select_time_pass_t<type>*>(
								static_cast<TimePass<type>*>(passes.get<0>()))));
			}
		};
	}

	template<class ...Passes>
	struct ConcatTimePass : public detail::TimeMessure
	{
	private:
		static constexpr size_t num = sizeof...(Passes);
		boaaa::data_store<TimePass<Passes>*...> passes;

	public:		
		ConcatTimePass() : detail::TimeMessure(), passes({ new TimePass<Passes>()... }) { }

		void addPass(LLVMLegacyPassManager& pm) {
			recursiveCaller<num - 1, Passes...>::addPasses(pm, passes);
		}

		std::string identifyer() {
			return recursiveCaller<num - 1, Passes...>::unifyNames();
		}

		void unifyTime() override {
			detail::TimeMessure::reset();
			recursiveCaller<num - 1, Passes...>::unifyTime(*this, passes);
		}

		void printResult(std::ostream& stream) override {
			stream << "TimePass Report for " << identifyer() << ":\n";
			unifyTime();
			detail::TimeMessure::printResult(stream);
		}

		void printToEvalRes(EvaluationResult& er) override {
			std::string s = identifyer().c_str();
			er.set_aa_name(boaaa::copyString(s.c_str(), s.size()));
			er.set_aa_name_delete(boaaa::delete_mem);
			unifyTime();
			detail::TimeMessure::printToEvalRes(er);
		}
	};

}

#endif //!BOAAA_LV_TIMEPASS_H