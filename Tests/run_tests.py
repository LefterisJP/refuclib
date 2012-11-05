# This is the main testing script for the Refu library project
# All examples and tests are compiled and tested, being compared with
# the expected output. At the end of the script a list with the results of all
# the tests denoting which ones were a success and which (if any) failed.


import sys
from libcompile import compileLib
from tests import runTests
from configreader import Config
from compiler import Compiler
from args import Args




#read the command line arguments
args = Args();
#open the logfile
logFile = open(args.testsLogName,"w")
#for each different compiler
for cName in args.compilers:
    print("===Running Tests for the {0} Compiler===\n".format(cName))
    #Read in the configuration file of the system
    try:
        config = Config("..");#the relative path of the configuration file
    except Exception as err:
        print(err);
        print("Failure at configuration file parsing. Testing scripts can't continue...Quitting")
        sys.exit(-1);
    #Read in the compiler data to know how to compile each test
    try:
        compiler = Compiler(config);
    except Exception as err:
        print(err);
        print("Failure at reading the {0} compiler's data. Skipping this compiler...")
        continue;
    #compile a dynamic version of the library
    if(compileLib(args.verbose,True,config,compiler)==False):
        print("Compiling dynamic version of Refu with the {0} compiler failed. Skipping tests for this version...".format(cName));
    else:
        runTests(config,compiler,True,logFile,args.verbose,args.debug,args.tests);
    #compile a static version of the library
    if(compileLib(args.verbose,False,config,compiler)==False):
        print("Compiling static version of Refu with the {0} compiler failed. Skipping tests for this version...".format(cName));
    else:
        runTests(config,compiler,False,logFile,args.verbose,args.debug,args.tests);

print("\nAll tests have concluded. For more information you can check logfile: \"{0}\"".format(args.testsLogName));
logFile.close()

