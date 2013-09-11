from subprocess import CalledProcessError
import os
import shutil
import time
import platform

from tests_module import tests_list
from output import (print_nonl, report, TestError, TestsFail, TestCompileError)
from log import check_library_log

#the time by which to delay actions after compiling so as not to have conflicts
#after copying/deleting binaries
delay = 0.20


def configureLinking(dynamic, root, outName):
    """Deals with the library linking for each test"""
    if(dynamic):
        #if we are testing the dynamic lib in windows copy it to the
        #test  directory source so that the lib can be accessed
        if(platform.system() == 'Windows'):
            try:
                os.remove(os.path.join(root, outName))
            except:
                pass
            shutil.copy(os.path.join(".", outName),
                        os.path.join(root, outName))


def runTests(compiler, dynamic, outName, logFile, verbose,
             debug, requested_tests, fail_fast, keep_exec):
    """Runs the Tests of the Refu library

        --compiler: The name of the compiler
        --dynamic: A boolean flag, true for using a dynamic version of
                   the library and false for static
        --outName: The full filename of the compiled refu library with
                   which to link
        --logFile: The log file to keep so that the user can see an
                   overview of how the tests are performed.
        --verbose: A boolean flag denoting whether the function should
                   be verbose about its actions or not
        --debug: A boolean flag denoting whether the tests should be in
                 debug mode (no testing or comparison) or not
        --requested_tests: A list of specific tests to run. If it is empty all
                 tests are run. If it is not then only the specified
                 ones are.
        --fail_fast: A boolean flag denoting if the testing scripts should
                     stop right after a failure or not
        --keep_exec: A boolean flag denoting if the executable of the test
                     should be kept and not deleted under some circumstances
    """
    #test executable name is always test for now
    if(platform.system() == 'Windows'):
        testExec = 'test.exe'
    else:
        testExec = 'test'

    libtype_str = "static"
    if(dynamic):
        libtype_str = "dynamic"

    report(logFile,
           "===> Starting Tests for the {} version of Refu library with "
           " the \"{}\" Compiler\n\n".format(libtype_str, compiler))

    # Recursively iterate all the files in the Tests directory
    for test in tests_list:
        # check to see if the user provided specific tests, and if
        # he did check to see if this is one of them
        if(requested_tests and test.name not in requested_tests):
                continue
        #if there is any action to do before compiling do it
        test.before_compile()

        # compile it
        try:
            test.compile_test(dynamic, compiler, verbose, logFile)
        except TestCompileError as e:
            print(e)
            if fail_fast:
                raise TestsFail(
                    "Test \"{}\" failed and --fail"
                    "-fast was requested".format(test.name))

            print("Continuing with the compilation of the other tests")
            continue  # if it fails go to the next test

        # after each compile add a small delay since the
        # subsequent copy may fail
        time.sleep(delay)
        # move the executable to the specific test directory its
        #source was and if there was one there already delete it
        try:
            os.remove(os.path.join(test.root, testExec))
        except:
            pass
        os.rename(os.path.join(".", testExec),
                  os.path.join(test.root, testExec))
        #deal with the library linking for each test
        configureLinking(dynamic, test.root, outName)

        #test it
        try:
            if(debug):
                test.debug_test(logFile, testExec)
            else:
                test.run_test(logFile, testExec, verbose, fail_fast)
            # check the log for errors
            check_library_log(logFile, test.root, test.name)
        except TestError as err:
            #if there was a test error print it
            print(err)
            if fail_fast:
                raise TestsFail(
                    "Test \"{}\" failed and --fail"
                    "-fast was requested".format(test.name))
        except CalledProcessError as err:
            print("\tThere was an error while trying to run the "
                  "compiled "
                  "executable for test \"{}\"".format(test.name))
            print("\tThe output was:")
            print(err.output)
            logFile.write("\tTest \"{}\" failed due to "
                          "inability to run the "
                          "executable\n".format(test.name))
            if fail_fast:
                raise TestsFail(
                    "Test \"{}\" failed and --fail"
                    "-hard was requested".format(test.name))
        finally:
            # cleaning up
            test.after_test()

            if dynamic:
                # also if we are testing the dynamic lib,
                # delete its local copy
                try:
                    os.remove(os.path.join(test.root, outName))
                except:
                    pass
            #after finishing a test delete the executable
            if((not keep_exec and fail_fast)
               or
               (not keep_exec and len(requested_tests) != 1)):
                try:
                    os.remove(os.path.join(test.root, testExec))
                except:
                    pass
            else:
                print("\t**Keeping the executable of the test "
                      "as requested")
            # adding some delay between tests until I figure out
            # a better way to do this
            time.sleep(delay)
    #after all tests have concluded clean up
    print_nonl("\n")
    logFile.write("\n")

    # clean the tests directory from object files
    for root, dirs, files in os.walk("."):
        for f in files:
            if(f.endswith(('.o', '.obj'))):
                os.remove(os.path.join(root, f))
    try:
        os.remove(os.path.join('.', testExec))
    except:
        pass
    os.remove(os.path.join('.', outName))
