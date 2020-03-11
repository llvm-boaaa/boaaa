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
BoaaaCat("BOAAA options:");

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
AllVersions("all-Ver", cl::desc("If Flag is set, run all Analysis if available under all loaded versions"),
	cl::init(false), cl::cat(BoaaaCat));

static cl::opt<bool>
AllAnalysis("all-AAs", cl::desc("If Flag is set, run all Analysis, for each version"),
	cl::init(false), cl::cat(BoaaaCat));

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
std::shared_ptr<boaaa::DLInterface> llvm90;

boaaa::LLVMVersionManager *llvm_man = nullptr;
boaaa::StringRefVPM *sr_vp_man = nullptr;

boaaa::llvm_version versions_count = 0;
boaaa::llvm_version *versions_loaded;

llvm::StringMap<boaaa::cl_aa_store> aa_cl_group;

std::vector<boaaa::aa_id>* aa_map;
uint32_t aa_map_size;

#ifdef DEBUG
int testmain();
#endif // DEBUG

enum class COROUTINESTATES_MAIN : int8_t
{
	ERROR_IN_ANALYSIS			= -3,
	SKIP_ARGUMENTS				= -2,
	ERROR_WHILE_LOAD_MODULE		= -1,
	NORMAL						= 0,

};

COROUTINESTATES_MAIN mainloop();

void setup();
void initAAs();
void finalize();

int main(int argc, char** argv) {
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

	if (!FileInput.isDefaultOption()) { //no inputflie set because cl::init(-)
		res = static_cast<int8_t>(mainloop());
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
		cl::ResetAllOptionOccurrences();
		line_count++;
		int _argc;
		char** _argv = boaaa::parse(boaaa_string, line.data(), &_argc);
		if (_argc < 0) {
			std::cout << "Error: couldn`t seperate arguments of line:\n" << line << "\n";
			res--;
			continue;
		}
		cl::ParseCommandLineOptions(_argc, _argv);
		using CSM = COROUTINESTATES_MAIN;
		if (auto state = mainloop(); static_cast<int8_t>(state) < 0) {
			switch (state) {
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
			res--;
		}
		free(_argv); //delete them after mainloop, so all variables could be invalidated.
	}
	finalize();
	return res;
}

bool is_active_LLVM_40() {
	return Version.getValue() == llvm40->getVersion();
}

bool is_active_LLVM_50() {
	return Version.getValue() == llvm50->getVersion();
}

bool is_active_LLVM_90() {
	return Version.getValue() == llvm50->getVersion();
}

bool loadModule()
{
	uint64_t filehash = llvm_man->registerData(InputFilename);
	uint64_t prefixhash = 0;

	bool prefix = false;
	if (!PrefixFilePath.isDefaultOption()) {
		prefix = true;
		prefixhash = llvm_man->registerData(PrefixFilePath);
	}

	bool error;
	if (llvm40 && (AllVersions.getValue() || is_active_LLVM_40())) {
		if (prefix) error = llvm40->loadModule(prefixhash, filehash);
		else        error = llvm40->loadModule(filehash);
		if (error)
		{
			std::cout << "       Error in Module llvm 40" << "\n";
			return -1;
		}
	}

	if (llvm50 && (AllVersions.getValue() || is_active_LLVM_50())) {
		if (prefix) error = llvm50->loadModule(prefixhash, filehash);
		else        error = llvm50->loadModule(filehash);
		if (error)
		{
			std::cout << "       Error in Module llvm 50" << "\n";
			return -1;
		}
	}

	if (llvm90 && (AllVersions.getValue() || is_active_LLVM_90())) {
		if (prefix) error = llvm90->loadModule(prefixhash, filehash);
		else        error = llvm90->loadModule(filehash);
		if (error)
		{
			std::cout << "       Error in Module llvm 90" << "\n";
			return -1;
		}
	}
	return 0;
}

boaaa::aa_id getCurrentVersionId()
{
	using LLV = boaaa::LLVM_VERSIONS;

	switch (Version.getValue()) {
	case 40:
		return static_cast<boaaa::aa_id>(LLV::LLVM_40);
	case 50:
		return static_cast<boaaa::aa_id>(LLV::LLVM_50);
	case 90:
		return static_cast<boaaa::aa_id>(LLV::LLVM_90);
	default:
		return 0;
	}
}

bool runAnalysis(std::set<boaaa::aa_id> analysises)
{
	using LLV = boaaa::LLVM_VERSIONS;

	bool res = true;
	for (boaaa::aa_id aa : analysises)
	{
		switch (aa & boaaa::version_mask)
		{
		case LLV::LLVM_40:
			res &= llvm40->runAnalysis(aa);
			break;
		case LLV::LLVM_50:
			res &= llvm50->runAnalysis(aa);
			break;
		case LLV::LLVM_90:
			res &= llvm90->runAnalysis(aa);
			break;
		}
	}
	return res;
}

COROUTINESTATES_MAIN mainloop() 
{
	using CSM = COROUTINESTATES_MAIN;

	static CSM coroutine_state = CSM::NORMAL;
	
	switch (coroutine_state) {
		case CSM::ERROR_WHILE_LOAD_MODULE:
			coroutine_state = CSM::SKIP_ARGUMENTS;
		case CSM::SKIP_ARGUMENTS:
			if (!InputFilename.isDefaultOption())
				return coroutine_state; //skip because of previos error
		case CSM::ERROR_IN_ANALYSIS:	//error in analysis is not a big deal, for running other analysises, so run like normal behavior
		case CSM::NORMAL:
			if (!InputFilename.isDefaultOption())
			{
				if (loadModule()) 
				{
					coroutine_state = CSM::ERROR_WHILE_LOAD_MODULE;
					return coroutine_state; //error while laod module
				}
			}

			std::set<boaaa::aa_id> analysis_to_run;

			if (AllAnalysis.getValue())
			{
				if (AllVersions.getValue()) {
					for (int i = 0; i < aa_map_size; i++) {
						analysis_to_run.insert(std::begin(aa_map[i]), std::end(aa_map[i]));
					}
				}
				else 
				{
					if (!Version.isDefaultOption()) {
						for(boaaa::aa_id i = 0; i < aa_map_size; i++)
						{
							boaaa::aa_id id = getCurrentVersionId();
							for (boaaa::aa_id aa : aa_map[i])
								if ((aa & boaaa::version_mask) == id)
									analysis_to_run.insert(aa);
						}
					}
				}
			} 
			else
			{
				if (aa_list.size() > 0) {
					analysis_to_run.insert(std::begin(aa_list), std::end(aa_list));
				}
				if (version_list.size() > 0) {
					for (boaaa::aa_id i : version_list) 
						analysis_to_run.insert(std::begin(aa_map[i]), std::end(aa_map[i]));
				}
			}
			if (!runAnalysis(analysis_to_run)) {
				coroutine_state = CSM::ERROR_IN_ANALYSIS;
				return coroutine_state;
			}
	}

	coroutine_state = CSM::NORMAL;
	return coroutine_state;
}

#ifdef DEBUG
void passManagerTest()
{
	llvm::LLVMContext context;
	llvm::SMDiagnostic Err;
	std::unique_ptr<llvm::Module> module = llvm::parseIRFile(InputFilename, Err, context);

}

int testmain() {

	passManagerTest();

	StringRef test_string = "says hello";

	uint64_t str_test_hash = llvm_man->registerData(test_string);
	uint64_t test_bc = llvm_man->registerData(InputFilename);

	uint64_t* test_args = new uint64_t[2]{ str_test_hash, test_bc };

	if (!llvm40->loadModule(test_bc)) {
		std::cout << "       Error in Module llvm 40" << "\n";
	}

	if (!llvm50->loadModule(test_bc)) {
		std::cout << "       Error in Module llvm 50" << "\n";
	}

	if (!llvm90->loadModule(test_bc)) {
		std::cout << "       Error in Module llvm 90" << "\n";
	}

	llvm40->test(test_args, 2);
	llvm50->test(test_args, 2);
	llvm90->test(test_args, 2);

	return 0;
};

#endif //!DEBUG

void setupLLVMVersion(boaaa::DLInterface &handle, std::vector<boaaa::llvm_version>& v, uint16_t version) {
	v.push_back(version);

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

	llvm40 = llvm_man->loadDL("boaaa.lv_40");
	llvm50 = llvm_man->loadDL("boaaa.lv_50");
	llvm90 = llvm_man->loadDL("boaaa.lv_90");

	std::vector<boaaa::llvm_version> versions;

	if(llvm40)
		setupLLVMVersion(*llvm40, versions, llvm40->getVersion());
	if (llvm50)
		setupLLVMVersion(*llvm50, versions, llvm50->getVersion());
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
	//memory leak, but is only called once so it should be ok
	boaaa::aa_id id = regAA.get<1>() & boaaa::version_mask;
	switch (id) {
	case boaaa::LLVM_30:
		return strcatdup("30-", regAA.get<0>());
	case boaaa::LLVM_35:
		return strcatdup("35-", regAA.get<0>());
	case boaaa::LLVM_40:
		return strcatdup("40-", regAA.get<0>());
	case boaaa::LLVM_50:
		return strcatdup("50-", regAA.get<0>());
	case boaaa::LLVM_60:
		return strcatdup("60-", regAA.get<0>());
	case boaaa::LLVM_70:
		return strcatdup("70-", regAA.get<0>());
	case boaaa::LLVM_80:
		return strcatdup("80-", regAA.get<0>());
	case boaaa::LLVM_90:
		return strcatdup("90-", regAA.get<0>());
	default:
		return "illegal-definition";
	}
}

std::string getVersionString(boaaa::registeredAA regAA)
{
	boaaa::aa_id id = regAA.get<1>() & boaaa::version_mask;
	switch (id) {
	case boaaa::LLVM_30:
		return "30 ";
	case boaaa::LLVM_35:
		return "35 ";
	case boaaa::LLVM_40:
		return "40 ";
	case boaaa::LLVM_50:
		return "50 ";
	case boaaa::LLVM_60:
		return "60 ";
	case boaaa::LLVM_70:
		return "70 ";
	case boaaa::LLVM_80:
		return "80 ";
	case boaaa::LLVM_90:
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

	llvm90.reset();
	llvm50.reset();
	llvm40.reset();
	delete llvm_man;
}