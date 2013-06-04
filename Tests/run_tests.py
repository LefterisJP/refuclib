# This is the main testing script for the Refu library project
# All examples and tests are compiled and tested, being compared with
# the expected output. At the end of the script a list with the results of all
# the tests denoting which ones were a success and which (if any) failed.


import sys
from libcompile import compileLib
from tests import runTests
from args import parseArguments




#read the command line arguments
args = parseArguments()

#open the logfile
logFile = open(args.logfile, "w")
#for each different compiler
for compiler in args.compilers:
    print("===Running Tests for the {0} Compiler===\n".format(compiler))

    #compile a dynamic version of the library
    outName = compileLib(args.verbose, True, compiler)
    if(outName is False):
        print("Compiling dynamic version of Refu with the {0} compiler "
              "failed. Skipping tests for this version...".format(compiler))
    else:
        runTests(compiler, True, outName, logFile, args.verbose,
                 args.debug, args.tests, args.fail_fast)
    #compile a static version of the library
    outName = compileLib(args.verbose, False, compiler)
    if(outName is False):
        print("Compiling static version of Refu with the {0} compiler "
              "failed. Skipping tests for this version...".format(compiler))
    else:
        runTests(compiler, False, outName, logFile, args.verbose,
                 args.debug, args.tests, args.fail_fast)

print("\nAll tests have concluded. For more information you can check "
      "logfile: \"{0}\"".format(args.logfile))
logFile.close()

