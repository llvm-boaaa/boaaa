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


#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LegacyPassNameParser.h"
#include "llvm/IR/RemarkStreamer.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Target/TargetMachine.h"
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

static::cl::opt<std::string> FileInput("f", cl::desc("Specifies the file of multiple commandline arguments"), 
	cl::value_desc("path/filename.cline"), cl::cat(BoaaaCat));

static cl::opt<std::string> 
LogFilename("log", cl::desc("Specify output filename"), 
	cl::value_desc("path/filename"), cl::cat(BoaaaCat));


//copyied from opt

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
	cl::init("-"), cl::value_desc("filename"), cl::cat(BoaaaCat));


std::shared_ptr<boaaa::DLInterface> llvm40;
std::shared_ptr<boaaa::DLInterface> llvm50;
std::shared_ptr<boaaa::DLInterface> llvm90;

#ifdef DEBUG
int testmain();
#endif // DEBUG
int mainloop();

int main(int argc, char** argv) {
	//init llvm and register needed passes
#ifdef DEBUG
	const int _argc = 4;
	const char* _argv[] = { boaaa_string, "../../../../bc_sources/libbmi160.a.bc", "--debug-pass=Structure", "--evaluate-aa-metadata" }; //"--print-all-alias-modref-info" };
	//const char* _argv[] = { "boaaa", "-help" };
	cl::ParseCommandLineOptions(_argc, _argv);
	return testmain();
#endif

	cl::ParseCommandLineOptions(argc, argv);

	if (FileInput.isDefaultOption()) { //no inputflie set because cl::init(-)
		return mainloop();
	}

	fs::path inputfile = FileInput.c_str();
	
	if (!fs::exists(inputfile)) {
		std::cout << "InputFile: " << inputfile << " doesn`t exist!\n";
		return -1;
	}
	
	if (!(inputfile.has_extension() && inputfile.extension() == ".cline")) {
		std::cout << "Inputfile: " << inputfile << " doesn`t have the correct file extension .cline!";
		return -1;
	}
	
	if (!(inputfile.is_absolute())) {
		inputfile = fs::absolute(inputfile);
	}

	std::ifstream cl_inst(inputfile.c_str(), std::ios_base::in);
	std::string line;
	int res;
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
	return res;
}

//tea dsa branch of sea dsa
//svf

int mainloop() {



	return 0;
}

#ifdef DEBUG
int testmain() {
	//setup dlls

	boaaa::LLVMVersionManager man = boaaa::LLVMVersionManager();
	boaaa::StringRefVPM* srvpm = new boaaa::StringRefVPM();
	man.registerStringRefVPM(srvpm);

	llvm40 = man.loadDL("boaaa.lv_40");
	llvm50 = man.loadDL("boaaa.lv_50");
	llvm90 = man.loadDL("boaaa.lv_90");

	llvm40->setBasicOStream(std::cout);
	llvm50->setBasicOStream(std::cout);
	llvm90->setBasicOStream(std::cout);

	llvm40->registerStringRefVPM(srvpm);
	llvm50->registerStringRefVPM(srvpm);
	llvm90->registerStringRefVPM(srvpm);

	//main

	StringRef ref = "this is a test string";

	uint64_t str_test_hash = man.registerData(ref);
	uint64_t test_bc = man.registerData(InputFilename);

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
}

#endif //!DEBUG