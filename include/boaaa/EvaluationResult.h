#ifndef BOAAA_EVALUATION_RESULT_H
#define BOAAA_EVALUATION_RESULT_H

#include "boaaa/support/struct_help.h"
#include "boaaa/support/__STRINGIFY.h"

#include "rapidjson/document.h"

#include <functional>

//dont use namespace rapidjson, never, it destroyes llvm namespace!!!
//using namespace rapidjson;

typedef void(*delete_string)(char*);

namespace boaaa {
	inline char* copyString(const char* source, size_t len)
	{
		char* buffer = new char[len + 1];
		std::strcpy(buffer, source);
		return buffer;
	}

	inline void delete_mem(char* mchar)
	{
		delete[] mchar;
	}
}

#ifndef BOAAA_ADD_TIME_MEMBERS
#define BOAAA_ADD_TIME_MEMBERS(BASE)																\
BOAAA_SETTER_GETTER(uint64_t, BASE##_seconds)														\
BOAAA_SETTER_GETTER(uint16_t, BASE##_millis)														\
BOAAA_SETTER_GETTER(uint16_t, BASE##_micros)														\
BOAAA_SETTER_GETTER(uint16_t, BASE##_nanos)
#endif

//if (results.HasMember(id.c_str()))
//results[id.c_str()].SetObject();
//else
//{
//	rapidjson::Value& val = rapidjson::Value().SetObject();
//	results.AddMember(rapidjson::StringRef(id.c_str()), val.Move(), doc.GetAllocator());
//}

#ifndef BOAAA_CHECK_AND_WRITE
#define BOAAA_CHECK_AND_WRITE(VAR, MEMVAR, ID, SETFUNC, ALLOC)									\
if (VAR.HasMember(ID))																			\
	{ if ( ! VAR[ID].IsObject()) VAR[ID].SetObject(); }											\
else {																							\
	rapidjson::Value& val = rapidjson::Value().SetObject();										\
	VAR.AddMember(rapidjson::Value(ID, ALLOC), val.Move(), ALLOC);								\
} VAR[ID].SETFUNC(MEMVAR);
#endif

#ifndef BOAAA_CHECK_AND_WRITE_TIME
#define BOAAA_CHECK_AND_WRITE_TIME(VAR, BASE, ALLOC)												\
BOAAA_CHECK_AND_WRITE(VAR, m_##BASE##_seconds, __STRINGIFY(sec_##BASE), SetUint64, ALLOC)			\
BOAAA_CHECK_AND_WRITE(VAR, m_##BASE##_millis,  __STRINGIFY(mil_##BASE), SetUint  , ALLOC)			\
BOAAA_CHECK_AND_WRITE(VAR, m_##BASE##_micros,  __STRINGIFY(mic_##BASE), SetUint  , ALLOC)			\
BOAAA_CHECK_AND_WRITE(VAR, m_##BASE##_nanos,   __STRINGIFY(nan_##BASE), SetUint  , ALLOC)
#endif

#ifndef BOAAA_CHECK_AND_ASIGN
#define BOAAA_CHECK_AND_ASIGN(VAR, _BOOL, TMPBOOL, MEMVAR, ID, CHECKFUNC, GETFUNC)					\
TMPBOOL = VAR.HasMember( ID ) && VAR[ID].CHECKFUNC();												\
_BOOL |= TMPBOOL;																					\
if (TMPBOOL) MEMVAR = VAR[ID].GETFUNC();
#endif

#ifndef BOAAA_CHECK_AND_ASIGNCAST
#define BOAAA_CHECK_AND_ASIGNCAST(VAR, _BOOL, TMPBOOL, MEMVAR, ID, CHECKFUNC, GETFUNC, CAST)		\
TMPBOOL = VAR.HasMember( ID ) && VAR[ID].CHECKFUNC();												\
_BOOL |= TMPBOOL;																					\
if (TMPBOOL) MEMVAR = CAST(VAR[ID].GETFUNC());
#endif

#ifndef BOAAA_CHECK_AND_ASIGN_TIME(VAR, _BOOL, TMPBOOL, BASE)
#define BOAAA_CHECK_AND_ASIGN_TIME(VAR, _BOOL, TMPBOOL, BASE)																			\
BOAAA_CHECK_AND_ASIGN(    VAR, _BOOL, TMPBOOL, m_##BASE##_seconds, __STRINGIFY(sec_##BASE), IsUint64, GetUint64)						\
BOAAA_CHECK_AND_ASIGNCAST(VAR, _BOOL, TMPBOOL, m_##BASE##_millis,  __STRINGIFY(mil_##BASE), IsUint,   GetUint, static_cast<uint16_t>)	\
BOAAA_CHECK_AND_ASIGNCAST(VAR, _BOOL, TMPBOOL, m_##BASE##_micros,  __STRINGIFY(mic_##BASE), IsUint,   GetUint, static_cast<uint16_t>)	\
BOAAA_CHECK_AND_ASIGNCAST(VAR, _BOOL, TMPBOOL, m_##BASE##_nanos,   __STRINGIFY(nan_##BASE), IsUint,   GetUint, static_cast<uint16_t>)
#endif

namespace boaaa {

	struct EvaluationResult
	{
		BOAAA_SETTER_GETTER(char*, aa_name)
		BOAAA_SETTER(delete_string, aa_name_delete)

		//time definitions
		BOAAA_ADD_TIME_MEMBERS(pm_time)
		BOAAA_ADD_TIME_MEMBERS(analysis_time)
		BOAAA_ADD_TIME_MEMBERS(function_time)
		BOAAA_ADD_TIME_MEMBERS(alias_time)
		BOAAA_ADD_TIME_MEMBERS(modref_time)

		//alias
		BOAAA_SETTER_GETTER(uint64_t, alias_sum)
		BOAAA_SETTER_GETTER(uint64_t, no_alias_count)
		BOAAA_SETTER_GETTER(uint64_t, may_alias_count)
		BOAAA_SETTER_GETTER(uint64_t, partial_alias_count)
		BOAAA_SETTER_GETTER(uint64_t, must_alias_count)

		//modref
		BOAAA_SETTER_GETTER(uint64_t, modref_sum)
		BOAAA_SETTER_GETTER(uint64_t, no_modref_count)
		BOAAA_SETTER_GETTER(uint64_t, mod_count)
		BOAAA_SETTER_GETTER(uint64_t, ref_count)
		BOAAA_SETTER_GETTER(uint64_t, modref_count)
		BOAAA_SETTER_GETTER(uint64_t, must_count)
		BOAAA_SETTER_GETTER(uint64_t, must_mod_count)
		BOAAA_SETTER_GETTER(uint64_t, must_ref_count)
		BOAAA_SETTER_GETTER(uint64_t, must_modref_count)

		//alias set
		BOAAA_SETTER_GETTER(uint64_t, alias_sets)
		BOAAA_SETTER_GETTER(double, mean_alias_sets)
		BOAAA_SETTER_GETTER(double, var_alias_sets)

		BOAAA_SETTER_GETTER(uint64_t, ex_alias_sets)
		BOAAA_SETTER_GETTER(double, mean_ex_alias_sets)
		BOAAA_SETTER_GETTER(double, var_ex_alias_sets)

		BOAAA_SETTER_GETTER(uint64_t, no_alias_sets)
		BOAAA_SETTER_GETTER(double, mean_no_alias_sets)
		BOAAA_SETTER_GETTER(double, var_no_alias_sets)

		EvaluationResult() :
			m_aa_name(nullptr), m_aa_name_delete(nullptr),
			//time
			m_pm_time_seconds(0),       m_pm_time_millis(0),       m_pm_time_micros(0),       m_pm_time_nanos(0),
			m_analysis_time_seconds(0), m_analysis_time_millis(0), m_analysis_time_micros(0), m_analysis_time_nanos(0),
			m_function_time_seconds(0), m_function_time_millis(0), m_function_time_micros(0), m_function_time_nanos(0),
			m_alias_time_seconds(0),    m_alias_time_millis(0),    m_alias_time_micros(0),    m_alias_time_nanos(0),
			m_modref_time_seconds(0),   m_modref_time_millis(0),   m_modref_time_micros(0),   m_modref_time_nanos(0),
			//alias
			m_alias_sum(0), m_no_alias_count(0), m_may_alias_count(0), m_partial_alias_count(0), m_must_alias_count(0),
			//modref
			m_modref_sum(0), m_no_modref_count(0), m_mod_count(0), m_ref_count(0), m_modref_count(0), 
			m_must_count(0), m_must_mod_count(0), m_must_ref_count(0), m_must_modref_count(0),
			//alias sets
			m_alias_sets(0),    m_mean_alias_sets(0.0),    m_var_alias_sets(0.0),
			m_ex_alias_sets(0), m_mean_ex_alias_sets(0.0), m_var_ex_alias_sets(0.0),
			m_no_alias_sets(0), m_mean_no_alias_sets(0.0), m_var_no_alias_sets(0.0)
		{ }
		~EvaluationResult() { if (m_aa_name_delete) m_aa_name_delete(m_aa_name); }

		template<typename AllocatorType>
		rapidjson::Value& writeJson(rapidjson::Value& _value, AllocatorType& alloc)
		{
			if (!_value.IsObject()) _value.SetObject();

			const char* str = m_aa_name;

			if (_value.HasMember(str))
			{
				if (!_value[str].IsObject())
					_value[str].SetObject();
			}
			else
			{
				rapidjson::Value val = rapidjson::Value(rapidjson::kObjectType);
				_value.AddMember(rapidjson::Value(str, alloc), val, alloc);
				assert((_value.HasMember(str)));
			}

			rapidjson::Value& value = _value[str];

			//time

			BOAAA_CHECK_AND_WRITE_TIME(value, pm_time,			alloc)
			BOAAA_CHECK_AND_WRITE_TIME(value, analysis_time,    alloc)
			BOAAA_CHECK_AND_WRITE_TIME(value, function_time,	alloc)
			BOAAA_CHECK_AND_WRITE_TIME(value, alias_time,		alloc)
			BOAAA_CHECK_AND_WRITE_TIME(value, modref_time,		alloc)

			//time
			BOAAA_CHECK_AND_WRITE(value, m_alias_sum,							"alias_sum",				SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_no_alias_count,						"no_alias_count",			SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_may_alias_count,						"may_alias_count",			SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_partial_alias_count,					"partial_alias_count",		SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_partial_alias_count,					"partial_alias_count",		SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_partial_alias_count,					"partial_alias_count",		SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_must_alias_count,					"must_alias_count",			SetUint64, alloc)

			//modref
			BOAAA_CHECK_AND_WRITE(value, m_modref_sum,							"modref_sum",				SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_no_modref_count,						"no_modref_count",			SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_mod_count,							"mod_count",				SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_ref_count,							"ref_count",				SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_modref_count,						"modref_count",				SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_must_count,							"must_count",				SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_must_mod_count,						"must_mod_count",			SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_must_ref_count,						"must_ref_count",			SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_must_modref_count,					"must_modref_count",		SetUint64, alloc)

			//alias sets	
			BOAAA_CHECK_AND_WRITE(value, m_alias_sets,							"alias_sets",				SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_ex_alias_sets,						"ex_alias_sets",            SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_no_alias_sets,						"no_alias_sets",			SetUint64, alloc)
			if (m_alias_sets) {
				BOAAA_CHECK_AND_WRITE(value, m_mean_alias_sets,					"mean_alias_sets",			SetDouble, alloc)
				BOAAA_CHECK_AND_WRITE(value, m_var_alias_sets,					"var_alias_sets",			SetDouble, alloc)
			}

			if (m_ex_alias_sets) {
				BOAAA_CHECK_AND_WRITE(value, m_mean_ex_alias_sets,				"mean_ex_alias_sets",		SetDouble, alloc)
				BOAAA_CHECK_AND_WRITE(value, m_var_ex_alias_sets,				"var_ex_alias_sets",		SetDouble, alloc)
			}

			if (m_no_alias_sets) {
				BOAAA_CHECK_AND_WRITE(value, m_mean_no_alias_sets,				"mean_no_alias_sets",		SetDouble, alloc)
				BOAAA_CHECK_AND_WRITE(value, m_var_no_alias_sets,				"var_no_alias_sets",		SetDouble, alloc)
			}
			return value;
		}

		bool readJson(const rapidjson::Value& _value, const char* str)
		{
			bool tmp = false;
			bool result = true;

			if (!_value.IsObject()) return false;
			if (!_value.HasMember(str)) return false;
			if (!_value[str].IsObject()) return false;

			const rapidjson::Value& value = _value[str];

			//time 
			BOAAA_CHECK_AND_ASIGN_TIME(value, result, tmp, pm_time)
			BOAAA_CHECK_AND_ASIGN_TIME(value, result, tmp, analysis_time)
			BOAAA_CHECK_AND_ASIGN_TIME(value, result, tmp, function_time)
			BOAAA_CHECK_AND_ASIGN_TIME(value, result, tmp, alias_time)
			BOAAA_CHECK_AND_ASIGN_TIME(value, result, tmp, modref_time)

			//alias
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_alias_sum,				"alias_sum",				IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_no_alias_count,			"no_alias_count",			IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_may_alias_count,		"may_alias_count",			IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_partial_alias_count,	"partial_alias_count",		IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_must_alias_count,		"must_alias_count",			IsUint64, GetUint64)

			//modref
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_modref_sum,				"modref_sum",				IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_no_modref_count,		"no_modref_count",			IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_mod_count,				"mod_count",				IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_ref_count,				"ref_count",				IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_modref_count,			"modref_count",				IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_must_count,				"must_count",				IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_must_mod_count,			"must_mod_count",			IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_must_ref_count,			"must_ref_count",			IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_must_modref_count,		"must_modref_count",		IsUint64, GetUint64)

			//alias sets	
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_alias_sets,				"alias_sets",				IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_ex_alias_sets,			"ex_alias_sets",            IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_no_alias_sets,			"no_alias_sets",			IsUint64, GetUint64)

			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_mean_alias_sets,		"mean_alias_sets",			IsDouble, GetDouble)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_var_alias_sets,			"mean_alias_sets",			IsDouble, GetDouble)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_mean_ex_alias_sets,     "mean_ex_alias_sets",       IsDouble, GetDouble)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_var_ex_alias_sets,      "mean_ex_alias_sets",       IsDouble, GetDouble)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_mean_no_alias_sets,		"mean_no_alias_sets",		IsDouble, GetDouble)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_var_no_alias_sets,		"mean_var_alias_sets",		IsDouble, GetDouble)
				
			return result;
		}
	};

	struct ModuleResult
	{
		BOAAA_SETTER_GETTER(uint64_t, function_count)
		BOAAA_SETTER_GETTER(uint64_t, basic_block_count)
		BOAAA_SETTER_GETTER(uint64_t, loop_count)
		BOAAA_SETTER_GETTER(uint64_t, instruction_count)

		template<typename AllocatorType>
		rapidjson::Value& writeJson(rapidjson::Value& value, AllocatorType& alloc)
		{
			if (!value.IsObject()) value.SetObject();
		
			BOAAA_CHECK_AND_WRITE(value, m_function_count,			"function_count",		SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_basic_block_count,		"basic_block_count",	SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_loop_count,				"loop_count",			SetUint64, alloc)
			BOAAA_CHECK_AND_WRITE(value, m_instruction_count,		"instruction_count",	SetUint64, alloc)

			return value;
		}

		bool readJson(const rapidjson::Value& value)
		{
			bool tmp = false;
			bool result = true;

			if (!value.IsObject()) return false;
		
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_function_count,			"function_count",			IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_basic_block_count,		"basic_block_count",		IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_loop_count,				"loop_count",				IsUint64, GetUint64)
			BOAAA_CHECK_AND_ASIGN(value, result, tmp, m_instruction_count,		"instruction_count",		IsUint64, GetUint64)

			return result;
		}
	};
}

#undef BOAAA_ADD_TIME_MEMBERS
#undef BOAAA_CHECK_AND_WRITE
#undef BOAAA_CHECK_AND_WRITE_TIME
#undef BOAAA_CHECK_AND_ASIGN
#undef BOAAA_CHECK_AND_ASIGNCAST
#undef BOAAA_CHECK_AND_ASIGN_TIME

#include "boaaa/support/UNDEF__STRINGIFY.h"

#endif 