//debug flags
#define DEBUG 1

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#ifdef DEBUG
#define DEBUG_DLL_TEST

#endif

#define LLVM_VERSION_90 90
//boaaa
#include "boaaa/stdafx.h"

#include "llvm/Support/CommandLine.h"

#define NO_EXPORT //defines __export as nothing, to fix import problem of LLVMVersionManager
#include "boaaa/dynamic_interface.h"
#undef NO_EXPORT
#include "boaaa/lvm/LLVMVersionManager.h"
#include "boaaa/support/LLVMErrorOr.h"

#include <algorithm>
#include <memory>
#include <iostream>
#include <fstream>

#include "filesystem_check.h"
#include "command_line_seperator.h"
namespace fs = std::filesystem;

//using namespace llvm;


//command line arg variables

static char* boaaa_string = "boaaa";

static cl::OptionCategory BoaaaCat("BOAAA options:");

static::cl::opt<std::string> 
FileInput("f", cl::desc("Specifies the file of multiple commandline arguments"),
	cl::init("-"), cl::value_desc("path/filename.cline"), cl::cat(BoaaaCat));

static cl::opt<std::string> 
LogFilename("l", cl::desc("Specify log filename"), 
	cl::init("-"), cl::value_desc("path/filename"), cl::cat(BoaaaCat));

static cl::opt<uint16_t>
Version("-v", cl::desc("The version of the analysises"), 
	cl::init(0), cl::value_desc("XX") ,cl::cat(BoaaaCat));

static cl::opt<bool>
AllVersions("all-Ver", cl::desc("If Flag is set, run all Analysis if available under all loaded versions"),
	cl::init(false), cl::cat(BoaaaCat));

static cl::opt<bool>
AllAnalysis("all-AAs", cl::desc("If Flag is set, run all Analysis, for each version"),
	cl::init(false), cl::cat(BoaaaCat));



//copyied from opt

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
	cl::init("-"), cl::value_desc("path/filename"), cl::cat(BoaaaCat));



std::shared_ptr<boaaa::DLInterface> llvm40;
std::shared_ptr<boaaa::DLInterface> llvm50;
std::shared_ptr<boaaa::DLInterface> llvm90;

boaaa::LLVMVersionManager *llvm_man = nullptr;
boaaa::StringRefVPM *sr_vp_man = nullptr;

uint8_t versions_count = 0;
uint16_t *versions_loaded;

#ifdef DEBUG
int testmain();
#endif // DEBUG
int mainloop();

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

	if (FileInput.isDefaultOption()) { //no inputflie set because cl::init(-)
		res = mainloop();
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
	while (std::getline(cl_inst, line)) {
		cl::ResetAllOptionOccurrences();
		int _argc;
		char** _argv = boaaa::parse(boaaa_string, line.data(), &_argc);
		if (_argc < 0) {
			std::cout << "Error: couldn`t seperate arguments of line:\n" << line << "\n";
			res--;
			continue;
		}
		cl::ParseCommandLineOptions(_argc, _argv);
		res -= mainloop() < 0 ? 1 : 0;
		free(_argv); //delete them after mainloop, so all variables could be invalidated.
	}
	finalize();
	return res;
}

//tea dsa branch of sea dsa
//svf

int mainloop() {



	return 0;
}

#ifdef DEBUG
int testmain() {
	StringRef ref = "this is a test string";

	uint64_t str_test_hash = llvm_man->registerData(ref);
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

void setupLLVMVersion(boaaa::DLInterface &handle, std::vector<uint16_t>& v, uint16_t version) {
	v.push_back(version);

	handle.setBasicOStream(std::cout);
	if (!sr_vp_man) {
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

	std::vector<uint16_t> versions;

	if(llvm40)
		setupLLVMVersion(*llvm40, versions, llvm40->getVersion());
	if (llvm50)
		setupLLVMVersion(*llvm50, versions, llvm50->getVersion());
	if (llvm90)
		setupLLVMVersion(*llvm90, versions, llvm90->getVersion());

	versions_count = versions.size();
	versions_loaded = new uint16_t[versions_count];
	std::copy(versions.begin(), versions.end(), versions_loaded);
}

enum DummyClEnum : int32_t {
};

void addAAsToValuesClass(boaaa::cl_aa_store verAA, cl::list<DummyClEnum>& list, boaaa::llvm_version version)
{
	for (boaaa::registeredAA raa : verAA) {
		list.getParser().addLiteralOption(raa.get<0>(), raa.get<1>(), raa.get<2>());
	}
}

void initAAs()
{
	cl::list <DummyClEnum> aa_list(cl::desc("Choose witch Alias-Analysis-Passes to run:"));
	
	boaaa::cl_aa_store store;

	if (llvm40)
		addAAsToValuesClass(llvm40->getAvailableAAs(), aa_list, llvm40->getVersion());
	if (llvm50)
		addAAsToValuesClass(llvm50->getAvailableAAs(), aa_list, llvm50->getVersion());
	if (llvm90)
		addAAsToValuesClass(llvm90->getAvailableAAs(), aa_list, llvm90->getVersion());
	//cl::list<boaaa::aa_id> aa_list(cl::desc("Choose witch Alias-Analysis-Passes to run:"), val);
	//aa_list.addArgument();
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