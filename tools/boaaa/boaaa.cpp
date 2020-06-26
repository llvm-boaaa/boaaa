//debug flags
//#define DEBUG 1

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#ifdef DEBUG
#define DEBUG_DLL_TEST

#endif

//boaaa
#include "boaaa/stdafx.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/Support/CommandLine.h"

#define NO_EXPORT //defines __export as nothing, to fix import problem of LLVMVersionManager
#include "boaaa/dynamic_interface.h"
#undef NO_EXPORT
#include "boaaa/lvm/LLVMVersionManager.h"
#include "boaaa/support/LLVMErrorOr.h"
#include "boaaa/support/raw_type.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/writer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <set>
#include <string.h>
#include <utility>

#include "filesystem_check.h"
#include "command_line_seperator.h"
namespace fs = std::filesystem;

//using namespace llvm;


//command line arg variables

static cl::OptionCategory 
BoaaaCat("BOAAA options");

static::cl::opt<std::string> 
FileInput("f", cl::desc("Specifies the file of multiple commandline arguments"),
	cl::init("-"), cl::value_desc("path/filename.cline"), cl::cat(BoaaaCat));

static cl::opt<std::string> 
LogFilename("l", cl::desc("Specify log filename"), 
	cl::init("-"), cl::value_desc("path/filename"), cl::cat(BoaaaCat));

static cl::opt<std::string>
PrefixFilePath("p", cl::desc("The prefix-path of the Bitecodefile, it will get concatet to: prefix${llvm_version}InputFile"),
	cl::init(""), cl::value_desc("prefix"), cl::cat(BoaaaCat));

static cl::opt<int>
Version("v", cl::desc("The version of the analysises"), 
	cl::init(0), cl::value_desc("XX") ,cl::cat(BoaaaCat));

static cl::opt<bool>
AllVersions("all-Versions", cl::desc("If Flag is set, run all Analysis if available under all loaded versions"),
	cl::init(false), cl::cat(BoaaaCat));

static cl::opt<bool>
AllAnalysis("all-AAs", cl::desc("If Flag is set, run all Analysis, for each version"),
	cl::init(false), cl::cat(BoaaaCat));

static cl::opt<bool>
NoJsonExport("no-json", cl::desc("boaaa exports by default all results to a json: path/to/boaaa/results.json"),
	cl::init(false), cl::cat(BoaaaCat));

static cl::opt<bool> 
StoreAAResults("store", cl::desc("If Flag is set it stores all AAResuls for inter-Analysis comperison"), 
	cl::cat(BoaaaCat));

static cl::opt<bool>
TimeStamp("timestamp", cl::desc("Print a timestemp whenever call mainloop"),
	cl::cat(BoaaaCat));

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
	cl::init("-"), cl::value_desc("path/filename"), cl::cat(BoaaaCat));

enum DummyClEnum : boaaa::aa_id {
	DUMMY
};

//dynamic 
static cl::list <DummyClEnum> 
version_list(cl::desc("Choose which Alias-Analysis-Passes to run in all available versions:"), cl::cat(BoaaaCat));

static cl::list <DummyClEnum> 
aa_list(cl::desc("Choose which Alias-Analysis-Passes to run in specific version:"), cl::cat(BoaaaCat));

//global parameters

static char* boaaa_string = "boaaa";

std::shared_ptr<boaaa::DLInterface> llvm40;
std::shared_ptr<boaaa::DLInterface> llvm50;
std::shared_ptr<boaaa::DLInterface> llvm60;
std::shared_ptr<boaaa::DLInterface> llvm71;
std::shared_ptr<boaaa::DLInterface> llvm80;
std::shared_ptr<boaaa::DLInterface> llvm90;

boaaa::LLVMVersionManager *llvm_man = nullptr;
boaaa::StringRefVPM *sr_vp_man = nullptr;

boaaa::llvm_version versions_count = 0;
boaaa::llvm_version *versions_loaded;

llvm::StringMap<boaaa::cl_aa_store> aa_cl_group;

std::vector<boaaa::aa_id>* aa_map;
uint32_t aa_map_size;

rapidjson::Document doc;
rapidjson::Value results;

#ifdef DEBUG
int testmain();
#endif // DEBUG

enum class COROUTINESTATES_MAIN : int8_t
{
	ERROR_IN_ANALYSIS			= -3,
	SKIP_ARGUMENTS				= -2,
	ERROR_WHILE_LOAD_MODULE		= -1,
	NORMAL						= 0,
	INIT						= 1,
	NEXT_VERSION				= 2,
	NO_ARGS_LEFT				= 3,
};

COROUTINESTATES_MAIN mainloop();

void setup();
void initAAs();
void finalize();
void writeJson();
void writeTimeStemp();
void setStoreAAResults();

void evaluateMainloopError(COROUTINESTATES_MAIN cms, uint64_t line_count) {
	
	using CSM = COROUTINESTATES_MAIN;
	switch (cms) {
	case CSM::ERROR_WHILE_LOAD_MODULE:
		std::cout << "Error while Load Module in LIne: " << line_count << "\n";
		break;
	case CSM::SKIP_ARGUMENTS:
		std::cout << "Skipped Analysis defined in LIne: " << line_count << " because of failed Module loading\n";
		break;
	case CSM::ERROR_IN_ANALYSIS:
		std::cout << "Error in at least one Analysis of Line:" << line_count << "\n";
		break;
	}
}

int main(int argc, char** argv) {
	using CSM = COROUTINESTATES_MAIN;
	//init llvm and register needed passes
	int res = 0;
	setup();
	initAAs();

#ifdef DEBUG
	const int _argc = 2;
	const char* _argv[] = { boaaa_string, "-h" }; // "../../../../bc_sources/libbmi160.a.bc" };

	//const char* _argv[] = { "boaaa", "-help" };
	cl::ParseCommandLineOptions(_argc, _argv);
	res = testmain();
	finalize();
	return res;
#endif
	
	cl::ParseCommandLineOptions(argc, argv);
	setStoreAAResults();

	if (FileInput.compare(FileInput.getDefault().getValue()) == 0) { //no inputflie set because cl::init(-)
		for (CSM state = mainloop(); state != CSM::NO_ARGS_LEFT; state = mainloop()) {
			writeJson();
			if (static_cast<int8_t>(state) < 0) {
				evaluateMainloopError(state, 0);
				--res;
			}
		}
		finalize();
		return res;
	}

	fs::path inputfile = FileInput.c_str();
	
	if (!fs::exists(inputfile)) {
		std::cout << "InputFile: " << inputfile << " doesn`t exist!\n";
		finalize();
		return -1;
	}
	
	if (!(inputfile.has_extension() && inputfile.extension() == ".cline")) {
		std::cout << "Inputfile: " << inputfile << " doesn`t have the correct file extension .cline!";
		finalize();
		return -1;
	}
	
	if (!(inputfile.is_absolute())) {
		inputfile = fs::absolute(inputfile);
	}

	std::ifstream cl_inst(inputfile.c_str(), std::ios_base::in);
	std::string line;
	//while lines in file seperate arguments, parse them and run main loop to calculate what the arguments describe.
	uint64_t line_count = 0;
	while (std::getline(cl_inst, line)) {
		//skip empty lines and lines starting with //
		if (line.empty()) continue;
		if (line.rfind("//", 0) == 0) continue;
		//print all noncomment lines to see the input when running boaaa
		std::cout <<line_count << ": " << line << "\n";
		line_count++;

		std::string prefix(PrefixFilePath.getValue());
		std::string file(InputFilename.getValue());
		bool store = StoreAAResults.getValue();
		bool timestamp = TimeStamp.getValue();
		InputFilename.reset();

		cl::ResetAllOptionOccurrences();
		int _argc;
		char** _argv = boaaa::parse(boaaa_string, line.data(), &_argc);
		if (_argc < 0) {
			std::cout << "Error: couldn`t seperate arguments of line:\n" << line << "\n";
			res--;
			continue;
		}
		
		cl::ParseCommandLineOptions(_argc, _argv);
		if (InputFilename.getValue().compare(InputFilename.getDefault().getValue()) == 0)
			InputFilename.setValue(file);
		if (PrefixFilePath.getValue().compare(PrefixFilePath.getDefault().getValue()) == 0)
			PrefixFilePath.setValue(prefix);
		//if (StoreAAResults.getValue() != store) {
		if (StoreAAResults.getNumOccurrences() > 0) {
			if (StoreAAResults.getValue() != store) setStoreAAResults();
			//std::cout << "[Info] store AAResults: " << (StoreAAResults.getValue() ? "true" : "false") << "\n";
		} else {
			StoreAAResults.setValue(store);
		}
		if (TimeStamp.getNumOccurrences() == 0) {
			TimeStamp.setValue(timestamp);
		}


		for (CSM state = mainloop(); state != CSM::NO_ARGS_LEFT; state = mainloop()) {
			writeJson();
			if (TimeStamp.getValue()) writeTimeStemp();
			if (static_cast<int8_t>(state) < 0) {
				evaluateMainloopError(state, line_count);
				--res;
			}
		}
		free(_argv); //delete them after mainloop, so all variables could be invalidated.
	}
	finalize();
	return res;
}

bool is_active(std::shared_ptr<boaaa::DLInterface>& dl) {
	return Version.getValue() == dl->getVersion();
}

void checkObjectOrCreate(rapidjson::Value& value, const char* str)
{
	if (value.HasMember(str))
	{
		if (!value[str].IsObject())
			value[str].SetObject();
	}
	else
	{
		rapidjson::Value val = rapidjson::Value(rapidjson::kObjectType);
		value.AddMember(rapidjson::Value(str, doc.GetAllocator()), val, doc.GetAllocator());
		assert(value.HasMember(str));
	}
}

void loadModuleHelper(std::shared_ptr<boaaa::DLInterface>& dl,bool& error, bool prefix, boaaa::llvm_version version, uint64_t prefixhash, uint64_t filehash, rapidjson::Value& value)
{
	checkObjectOrCreate(value, dl->getVersionString());
	rapidjson::Value& version_obj = value[dl->getVersionString()];

	if (dl && version == dl->getVersion() && (AllVersions.getValue() || is_active(dl))) {
		if (prefix) error = dl->loadModule(prefixhash, filehash);
		else        error = dl->loadModule(filehash);
		if (!error)
		{
			std::cout << "       Error in Module llvm " << dl->getVersionString() <<"\n";
		} 
		else
		{
			boaaa::ModuleResult* mr = dl->getModuleResult();
			if (mr) mr->writeJson(version_obj, doc.GetAllocator());
		}
	}
}

bool loadModule(boaaa::llvm_version version)
{
	uint64_t filehash = llvm_man->registerData(InputFilename);
	uint64_t prefixhash = 0;

	bool prefix = false;
	if (!PrefixFilePath.empty()) {
		prefix = true;
		prefixhash = llvm_man->registerData(PrefixFilePath);
	}

	if (!results.IsObject()) results.SetObject();
	std::string id = fs::path(InputFilename.getValue().c_str()).stem().u8string();
	checkObjectOrCreate(results, id.c_str());

	bool error;
	loadModuleHelper(llvm40, error, prefix, version, prefixhash, filehash, results[id.c_str()]);
	loadModuleHelper(llvm50, error, prefix, version, prefixhash, filehash, results[id.c_str()]);
	loadModuleHelper(llvm60, error, prefix, version, prefixhash, filehash, results[id.c_str()]);
	loadModuleHelper(llvm71, error, prefix, version, prefixhash, filehash, results[id.c_str()]);
	loadModuleHelper(llvm80, error, prefix, version, prefixhash, filehash, results[id.c_str()]);
	loadModuleHelper(llvm90, error, prefix, version, prefixhash, filehash, results[id.c_str()]);
	
	return error;
}

void unloadModuleHelper(std::shared_ptr<boaaa::DLInterface>& dl, boaaa::llvm_version version)
{
	if (dl && version == dl->getVersion() && (AllVersions.getValue() || is_active(dl)))
		dl->unloadModule();
}

void unloadModule(boaaa::llvm_version version)
{
	unloadModuleHelper(llvm40, version);
	unloadModuleHelper(llvm50, version);
	unloadModuleHelper(llvm60, version);
	unloadModuleHelper(llvm71, version);
	unloadModuleHelper(llvm80, version);
	unloadModuleHelper(llvm90, version);
}

boaaa::aa_id getLLVMVersion(uint32_t version) {
	using LLV = boaaa::LLVM_VERSIONS;

	switch (version) {
	case 30:
		return static_cast<boaaa::aa_id>(LLV::LLVM_30);
	case 35:
		return static_cast<boaaa::aa_id>(LLV::LLVM_35);
	case 40:
		return static_cast<boaaa::aa_id>(LLV::LLVM_40);
	case 50:
		return static_cast<boaaa::aa_id>(LLV::LLVM_50);
	case 60:
		return static_cast<boaaa::aa_id>(LLV::LLVM_60);
	case 70:
		return static_cast<boaaa::aa_id>(LLV::LLVM_70);
	case 71:
		return static_cast<boaaa::aa_id>(LLV::LLVM_71);
	case 80:
		return static_cast<boaaa::aa_id>(LLV::LLVM_80);
	case 90:
		return static_cast<boaaa::aa_id>(LLV::LLVM_90);
	case 100:
		return static_cast<boaaa::aa_id>(LLV::LLVM_100);
	case 110:
		return static_cast<boaaa::aa_id>(LLV::LLVM_110);
	default:
		return 0;
	}
}

boaaa::aa_id getCurrentVersionId()
{
	return getLLVMVersion(Version.getValue());
}

void writeJson()
{
	std::ofstream ofs("results.json");
	rapidjson::OStreamWrapper osw(ofs);
	rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
	results.Accept(writer);
}

void runAnalysisHelper(std::shared_ptr<boaaa::DLInterface>& dl, boaaa::aa_id aa, const char* id, bool& res)
{
	boaaa::EvaluationResult er;
	rapidjson::Value& val = results[id];

	checkObjectOrCreate(val, dl->getVersionString());
	res &= !NoJsonExport.getValue()
		? dl->runAnalysis(aa, er)
		: dl->runAnalysis(aa);
	if (!NoJsonExport.getValue()) er.writeJson(val[dl->getVersionString()], doc.GetAllocator());
}

bool runAnalysis(boaaa::aa_id aa)
{
	using LLV = boaaa::LLVM_VERSIONS;

	if (!results.IsObject()) results.SetObject();
	std::string id = fs::path(InputFilename.getValue().c_str()).stem().u8string();
	checkObjectOrCreate(results, id.c_str());

	bool res = true;
	switch (aa & boaaa::version_mask)
	{
	case LLV::LLVM_40:
		if (!AllVersions.getValue() && !is_active(llvm40)) break;
		runAnalysisHelper(llvm40, aa, id.c_str(), res);
		break;
	case LLV::LLVM_50:
		if (!AllVersions.getValue() && !is_active(llvm50)) break;
		runAnalysisHelper(llvm50, aa, id.c_str(), res);
		break;
	case LLV::LLVM_60:
		if (!AllVersions.getValue() && !is_active(llvm60)) break;
		runAnalysisHelper(llvm60, aa, id.c_str(), res);
		break;
	case LLV::LLVM_71:
		if (!AllVersions.getValue() && !is_active(llvm71)) break;
		runAnalysisHelper(llvm71, aa, id.c_str(), res);
		break;
	case LLV::LLVM_80:
		if (!AllVersions.getValue() && !is_active(llvm80)) break;
		runAnalysisHelper(llvm80, aa, id.c_str(), res);
		break;
	case LLV::LLVM_90:
		if (!AllVersions.getValue() && !is_active(llvm90)) break;
		runAnalysisHelper(llvm90, aa, id.c_str(), res);
		break;
	default:
		res = false;
		std::cout << "Analysis not found aa_id: " << std::hex << aa << std::dec << "\n";
		break;
	}
	return res;
}

COROUTINESTATES_MAIN mainloop() 
{
	using CSM = COROUTINESTATES_MAIN;

	static CSM coroutine_state = CSM::INIT;
	static std::vector<boaaa::aa_id> analysis_to_run;
	static int16_t index = -1;
	static uint32_t aa_index = 0;
	
	switch (coroutine_state) {
	case CSM::ERROR_WHILE_LOAD_MODULE:
		coroutine_state = CSM::SKIP_ARGUMENTS;
	case CSM::SKIP_ARGUMENTS:
		if (!InputFilename.hasArgStr()) {
			coroutine_state = CSM::NO_ARGS_LEFT;
			return coroutine_state; //skip because of previos error
		}
	case CSM::NO_ARGS_LEFT:
	case CSM::INIT:
		analysis_to_run.clear(); //clear analysis bevore
		aa_index = 0;
		index = -1; //increment in NEXT_VERSION to 0
		{
			std::set<boaaa::aa_id> analysis_temp;
			//scan analysis to run
			if (AllAnalysis.getValue())
			{
				if (AllVersions.getValue()) {
					for (int i = 0; i < aa_map_size; i++) {
						analysis_temp.insert(std::begin(aa_map[i]), std::end(aa_map[i]));
					}
				}
				else
				{
					if (Version.getValue() == Version.getDefault().getValue()) {
						for (boaaa::aa_id i = 0; i < aa_map_size; i++)
						{
							boaaa::aa_id id = getCurrentVersionId();
							for (boaaa::aa_id aa : aa_map[i])
								if ((aa & boaaa::version_mask) == id)
									analysis_temp.insert(aa);
						}
					}
				}
			}
			else
			{
				if (aa_list.size() > 0) {
					analysis_temp.insert(std::begin(aa_list), std::end(aa_list));
				}
				if (version_list.size() > 0) {
					for (boaaa::aa_id i : version_list)
						analysis_temp.insert(std::begin(aa_map[i]), std::end(aa_map[i]));
				}
			}

			for (boaaa::aa_id id : analysis_temp)
				analysis_to_run.push_back(id);
		}

	case CSM::NEXT_VERSION:

		if (index >= 0 && index < versions_count)
			unloadModule(versions_loaded[index]);

		++index;

		if (!InputFilename.hasArgStr())
		{
			if (!loadModule(versions_loaded[index]))
			{
				coroutine_state = CSM::ERROR_WHILE_LOAD_MODULE;
				return coroutine_state; //error while laod module
			}
		}

	case CSM::ERROR_IN_ANALYSIS:	//error in analysis is not a big deal, for running other analysises, so run like normal behavior
	case CSM::NORMAL:

		boaaa::aa_id max_id = index + 1 < versions_count ?
			getLLVMVersion(static_cast<uint32_t>(versions_loaded[index + 1])) : INT32_MAX;

		//if below eval same as this comment
		//if (!analysis_to_run.size() || aa_index == analysis_to_run.size())
		if (!(analysis_to_run.size() - aa_index))
			break;

		boaaa::aa_id id = analysis_to_run[aa_index];

		if(
			id < max_id									//prevent to run analyses with no loaded module
			&& aa_index < analysis_to_run.size())	//prevent from running out of indices from vector	
		{
			if (!runAnalysis(id)) {
				coroutine_state = CSM::ERROR_IN_ANALYSIS;
				return coroutine_state;
			}
			++aa_index;
			coroutine_state = CSM::NORMAL;
			return coroutine_state;
		}
		coroutine_state = CSM::NEXT_VERSION;
		return coroutine_state;
	//case CSM::NO_ARGS_LEFT: do nothing
	}
	//unload loaded module
	unloadModule(versions_loaded[index]);

	coroutine_state = CSM::NO_ARGS_LEFT;
	return coroutine_state;
}

#ifdef DEBUG
int testmain() {
	return 0;
};

#endif //!DEBUG

void setupLLVMVersion(boaaa::DLInterface &handle, std::set<boaaa::llvm_version>& v, uint16_t version) {
	v.insert(version);

	handle.setBasicOStream(std::cout);
	if (!sr_vp_man) { //init only one time per execution
		sr_vp_man = new boaaa::StringRefVPM();
		llvm_man->registerStringRefVPM(sr_vp_man);
	}
	handle.registerStringRefVPM(sr_vp_man);
}

void setup()
{
	llvm_man = new boaaa::LLVMVersionManager();

	if (fs::exists("results.json")) {
		std::ifstream ifs("results.json");
		rapidjson::IStreamWrapper isw(ifs);
		doc.ParseStream(isw);
		results.CopyFrom(doc, doc.GetAllocator());
		if (!results.IsObject())
			results.SetObject();
	}

	llvm40 = llvm_man->loadDL("boaaa.lv_40");
	llvm50 = llvm_man->loadDL("boaaa.lv_50");
	llvm60 = llvm_man->loadDL("boaaa.lv_60");
	llvm71 = llvm_man->loadDL("boaaa.lv_71");
	llvm80 = llvm_man->loadDL("boaaa.lv_80");
	llvm90 = llvm_man->loadDL("boaaa.lv_90");

	//set because it is sorted
	std::set<boaaa::llvm_version> versions;

	if(llvm40)
		setupLLVMVersion(*llvm40, versions, llvm40->getVersion());
	if (llvm50)
		setupLLVMVersion(*llvm50, versions, llvm50->getVersion());
	if (llvm60)
		setupLLVMVersion(*llvm60, versions, llvm60->getVersion());
	if (llvm71)
		setupLLVMVersion(*llvm71, versions, llvm71->getVersion());
	if (llvm80)
		setupLLVMVersion(*llvm80, versions, llvm80->getVersion());
	if (llvm90)
		setupLLVMVersion(*llvm90, versions, llvm90->getVersion());

	versions_count = versions.size();
	versions_loaded = new boaaa::llvm_version[versions_count];
	std::copy(versions.begin(), versions.end(), versions_loaded);
}

void addAAsToAAMap(boaaa::cl_aa_store verAA)
{
	for (boaaa::registeredAA raa : verAA) {
		auto it = aa_cl_group.find(raa.get<0>());
		if (it != aa_cl_group.end())
		{
			it->getValue().push_back(raa);
		}
		else 
		{
			aa_cl_group.insert(std::make_pair(llvm::StringRef(raa.get<0>()), std::vector{ raa }));
		}
	}
}

char* strcatdup(char* c1, char* c2)
{
	size_t len_c1 = strlen(c1);
	size_t len_c2 = strlen(c2);
	char* cres = new char[len_c1 + len_c2 + 1];
	strcpy(cres, c1);
	strcpy(cres + len_c1, c2);
	return cres;
}

llvm::StringRef buildCLArg(boaaa::registeredAA regAA)
{
	using LLV = boaaa::LLVM_VERSIONS;
	//memory leak, but is only called once so it should be ok
	boaaa::aa_id id = regAA.get<1>() & boaaa::version_mask;
	switch (id) {
	case LLV::LLVM_30:
		return strcatdup("30-", regAA.get<0>());
	case LLV::LLVM_35:
		return strcatdup("35-", regAA.get<0>());
	case LLV::LLVM_40:
		return strcatdup("40-", regAA.get<0>());
	case LLV::LLVM_50:
		return strcatdup("50-", regAA.get<0>());
	case LLV::LLVM_60:
		return strcatdup("60-", regAA.get<0>());
	case LLV::LLVM_70:
		return strcatdup("70-", regAA.get<0>());
	case LLV::LLVM_71:
		return strcatdup("71-", regAA.get<0>());
	case LLV::LLVM_80:
		return strcatdup("80-", regAA.get<0>());
	case LLV::LLVM_90:
		return strcatdup("90-", regAA.get<0>());
	default:
		return "illegal-definition";
	}
}

std::string getVersionString(boaaa::registeredAA regAA)
{
	using LLV = boaaa::LLVM_VERSIONS;

	boaaa::aa_id id = regAA.get<1>() & boaaa::version_mask;
	switch (id) {
	case LLV::LLVM_30:
		return "30 ";
	case LLV::LLVM_35:
		return "35 ";
	case LLV::LLVM_40:
		return "40 ";
	case LLV::LLVM_50:
		return "50 ";
	case LLV::LLVM_60:
		return "60 ";
	case LLV::LLVM_70:
		return "70 ";
	case LLV::LLVM_71:
		return "71 ";
	case LLV::LLVM_80:
		return "80 ";
	case LLV::LLVM_90:
		return "90 ";
	default:
		return "illegal-definition";
	}
}

void addAAsToCL(cl::list<DummyClEnum>& version_list, cl::list<DummyClEnum>& aa_list)
{
	uint32_t i = 0;
	aa_map_size = aa_cl_group.size();
	aa_map = new std::vector<boaaa::aa_id>[aa_map_size];

	for (auto& elem : aa_cl_group)
	{
		std::string arg = elem.first();
		boaaa::cl_aa_store group = elem.second;
		std::string versions;
		int k = 0;
		switch (group.size())
		{
		case 0: continue; //should not happen
		default:
			for (boaaa::registeredAA reg : group)
			{
				//todo check crash, copy char* or something
				aa_list.getParser().addLiteralOption(buildCLArg(reg), reg.get<1>(), reg.get<2>());
				versions += (k++ == 0 ? "" : "\n   ") //adds \n before each line except the first one
							+ getVersionString(reg) + reg.get<2>();
				aa_map[i].push_back(reg.get<1>());
			}
			version_list.getParser().addLiteralOption(strdup(arg.c_str()), i++, strdup(versions.c_str()));
		}
	}
}

void initAAs()
{
	if (llvm40)
		addAAsToAAMap(llvm40->getAvailableAAs());
	if (llvm50)
		addAAsToAAMap(llvm50->getAvailableAAs());
	if (llvm60)
		addAAsToAAMap(llvm60->getAvailableAAs());
	if (llvm71)
		addAAsToAAMap(llvm71->getAvailableAAs());
	if (llvm80)
		addAAsToAAMap(llvm80->getAvailableAAs());
	if (llvm90)
		addAAsToAAMap(llvm90->getAvailableAAs());

	addAAsToCL(version_list, aa_list);
	version_list.addArgument();
	aa_list.addArgument();
	version_list.getParser().initialize();
	aa_list.getParser().initialize();
}

void finalize()
{
	if (sr_vp_man)
		delete sr_vp_man;

	llvm40.reset();
	llvm50.reset();
	llvm60.reset();
	llvm71.reset();
	llvm80.reset();
	llvm90.reset();
	delete llvm_man;

	writeJson();
}

void setStoreAAResults() {
	if (llvm40)
		llvm40->storeAAResults(StoreAAResults.getValue());
	if (llvm50)
		llvm50->storeAAResults(StoreAAResults.getValue());
	if (llvm60)
		llvm60->storeAAResults(StoreAAResults.getValue());
	if (llvm71)
		llvm71->storeAAResults(StoreAAResults.getValue());
	if (llvm80)
		llvm80->storeAAResults(StoreAAResults.getValue());
	if (llvm90)
		llvm90->storeAAResults(StoreAAResults.getValue());
}

void writeTimeStemp() {
	using timestamp = typename std::chrono::time_point<std::chrono::system_clock>;

	static bool first = false;
	static timestamp old = std::chrono::system_clock::now().operator-=(std::chrono::seconds(120));

	timestamp now = std::chrono::system_clock::now();

	//print timestamp if last is longer than 1 minute ago, prevents multi timestamps when skipping analysis and version.
	if (std::chrono::duration_cast<std::chrono::seconds>(now - old) > std::chrono::seconds(20)) {

		std::time_t now_t = std::chrono::system_clock::to_time_t(now);

		std::cout << "timestamp: " << std::ctime(&now_t) << "\n";
		old = now;
	}
}