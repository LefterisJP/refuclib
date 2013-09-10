import subprocess
from subprocess import CalledProcessError
import os
import shutil
import time
import platform
import sys


tests_dict = { 
    "dynamicarray": {
        "sources": [os.path.join("Data_Structures", "dynamicarray.c")],
        "root": "Data_Structures"
    },
    "hashmap": {
        "sources": [os.path.join("Data_Structures", "hashmap.c")],
        "root": "Data_Structures"
    },
    "intrusive_list": {
        "sources": [os.path.join("Data_Structures", "intrusive_list.c")],
        "root": "Data_Structures"
    },
    "list": {
        "sources": [os.path.join("Data_Structures", "list.c")],
        "root": "Data_Structures"
    },

    "string_accessor": {
        "sources": [os.path.join("String", "string_accessor.c")],
        "root": "String"
    },
    "string_additions": {
        "sources": [os.path.join("String", "string_additions.c")],
        "root": "String"
    },
    "string_conversion": {
        "sources": [os.path.join("String", "string_conversion.c")],
        "root": "String"
    },
    "string_copying": {
        "sources": [os.path.join("String", "string_copying.c")],
        "root": "String"
    },
    "string_filedescriptors": {
        "sources": [os.path.join("String", "string_filedescriptors.c")],
        "root": "String"
    },
    "string_format": {
        "sources": [os.path.join("String", "string_format.c")],
        "root": "String"
    },
    "string_init": {
        "sources": [os.path.join("String", "string_init.c")],
        "root": "String"
    },
    "string_iteration": {
        "sources": [os.path.join("String", "string_iteration.c")],
        "root": "String"
    },
    "string_positionalretrieval": {
        "sources": [os.path.join("String", "string_positionalretrieval.c")],
        "root": "String"
    },
    "string_removals": {
        "sources": [os.path.join("String", "string_removals.c")],
        "root": "String"
    },
    "string_replace": {
        "sources": [os.path.join("String", "string_replace.c")],
        "root": "String"
    },
    "string_retrieval": {
        "sources": [os.path.join("String", "string_retrieval.c")],
        "root": "String"
    },
    "string_traversal": {
        "sources": [os.path.join("String", "string_traversal.c")],
        "root": "String"
    },

    "system_files": {
        "sources": [os.path.join("System", "system_files.c")],
        "root": "System"
    },

    "textfile_lineend": {
        "sources": [os.path.join("TextFile", "textfile_lineend.c")],
        "root": "TextFile"
    },
    "textfile_read": {
        "sources": [os.path.join("TextFile", "textfile_read.c")],
        "root": "TextFile"
    },
    "textfile_write": {
        "sources": [os.path.join("TextFile", "textfile_write.c")],
        "root": "TextFile"
    }
}


from testscompile import compileTest
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
                os.remove(os.path.join(root,outName))
            except:
                pass
            shutil.copy(os.path.join(".",outName),os.path.join(root,outName))


def test(root, test_name, logFile, testExec, verbose, fail_fast):
    """
        Tests a compiled test executable and compared it to the expected
        output
        Arguments:

        --root: The root directory of the test compared to the Tests
                directory
        --fileName: The name of the test in the test_dict 
        (hence also the .expect)
        --logFile: The name of the logfile
        --testExec: The name of the test executable
        --verbose: A boolean flag denoting if the tests should be verbose
                  or not
        --fail_fast: A boolean flag denoting if the tests should fail fast
    """
    print_nonl("\t--> Testing \"{}\"...".format(test_name))
    logFile.write("\t--> Testing \"{}\"...".format(test_name))
    #open the expected outputfile
    f = open(os.path.join(root, "{}.expect".format(test_name)), 'r')
    #call the test
    p = subprocess.Popen(os.path.join('.',testExec),
                         cwd=root,stdout=subprocess.PIPE)
    expected_stdout_lineN = 1
    line = None
    for line in p.stdout:
        #read each line
        line = line.decode("utf8");
        line = line.replace("\r\n","\n")
        #if it's just a position reporting line log it and continue
        if "passed >>" in line:
            logFile.write("\t\t\t{}\n".format(line))
            continue
        #if not then let's check the expected stdout line
        expected_stdout = f.readline()
        if line != expected_stdout:
            # if it's an EXPECT() macro error, report it
            if "*ERROR*:" in line:
                report(logFile, "FAILED!\n\t {}".format(line))
                if fail_fast:
                    raise TestError(test_name, expected_stdout_lineN)
            # else it's an expected std output error
            else:
                report(logFile, "\n\t\tTest failed at expected "
                            "output line \"[{}]\"\n".format(
                                str(expected_stdout_lineN)))
                report(logFile, "\n\t\tExpected:\n\t\t\t{}\n\t\t"
                            "Found:\n\t\t\t{}\n".format(
                                str(expected_stdout), str(line)))
                raise TestError(test_name, expected_stdout_lineN)
            expected_stdout_lineN += 1
    #wait till the test subprocess ends
    ret = p.poll()
    while(ret is None):
        ret=p.poll()
    #success
    if ret is 0:
        report(logFile, "OK!\n")
    else:
        report(logFile, "FAILED!\n\t\tUnexpected Return Value. Test "
               "executable probably segfaulted.")
        if line is not None:
            line = line.partition("[")[2]
            line = line.partition("]")[0]
            report(logFile,
                   "\t\tLast line we are sure succesfully executed "
                   "is [{}]\n".format(str(line)))
        raise TestError(test_name, line)

    return True

def debugTest(root, test_name, logFile, testExec):
    """
        Debugs a compiled test executable by just spitting out its output
        --root: The root directory of the test compared to the Tests directory
        --test_name: The name of the test
        --logFile: The name of the logfile
        --testExec: The name of the test executable
    """
    report(logFile, 
           "\t**Testing \"{}\" in Debug Mode**\n".format(test_name))
    p = subprocess.Popen(os.path.join('.', testExec), cwd=root, 
                             stdout=subprocess.PIPE)
    for line in p.stdout:#for every line of the output
        line = line.decode("utf8")
        line = line.replace("\r\n", "\n")
        print_nonl("\t" + line)
        logFile.write(line)
    #wait till the subprocess ends
    ret = p.poll()
    while(ret is None):
        ret=p.poll()


def runTests(compiler, dynamic, outName, logFile, verbose,
             debug, tests, fail_fast, keep_exec):
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
        --tests: A list of specific tests to run. If it is empty all
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

    # clean the tests directory from object files
    for root, dirs, files in os.walk("."):
        for f in files:
            if(f.endswith(('.o','.obj'))):
                os.remove(os.path.join(root, f))

    # Recursively iterate all the files in the Tests directory
    for key, value in tests_dict.iteritems():
        # check to see if the user provided specific tests, and if
        # he did check to see if this is one of them
        if(tests and key not in tests):
                continue
        # compile it
        try:
            compileTest(key, value["sources"], dynamic, compiler, verbose,
                            logFile)
        except TestCompileError as e:
            print(e)
            if fail_fast:

                raise TestsFail(
                    "Test \"{}\" failed and --fail"
                    "-fast was requested".format(part))

            print("Continuing with the compilation of the other tests")
            continue#if it fails go to the next test

        # after each compile add a small delay since the
        # subsequent copy may fail
        time.sleep(delay)
        # move the executable to the specific test directory its
        #source was and if there was one there already delete it
        try:
            os.remove(os.path.join(value["root"], testExec))
        except:
            pass
        os.rename(os.path.join(".", testExec),
                  os.path.join(value["root"], testExec))
        #deal with the library linking for each test
        configureLinking(dynamic, value["root"], outName)

        #test it
        try:
            if(debug):
                debugTest(value["root"], key, logFile, testExec)
            else:
                test(value["root"], key, logFile, testExec,
                     verbose, fail_fast)

            # check the log for errors
            check_library_log(logFile, value["root"], key)
        except TestError as err:
            #if there was a test error print it
            print(err)
            if fail_fast:
                raise TestsFail(
                    "Test \"{}\" failed and --fail"
                    "-fast was requested".format(key))
        except CalledProcessError as err:
            print("\tThere was an error while trying to run the "
                  "compiled "
                  "executable for test \"{}\"".format(key))
            print("\tThe output was:")
            print(err.output)
            logFile.write("\tTest \"{}\" failed due to "
                          "inability to run the "
                          "executable\n".format(key))
            if fail_fast:
                raise TestsFail(
                    "Test \"{}\" failed and --fail"
                    "-hard was requested".format(key))
        finally:
            # cleaning up
            if dynamic:
                # also if we are testing the dynamic lib,
                # delete its local copy
                try:
                    os.remove(os.path.join(value["root"], outName))
                except:
                    pass
            #after finishing a test delete the executable
            if ( not keep_exec and fail_fast or 
                (not keep_exec and len(tests) != 1)):
                try:
                    os.remove(os.path.join(value["root"], testExec))
                except:
                    pass
            else:
                print("\t**Keeping the executable of the test "
                      "as requested")
            # adding some delay between tests until I figure out
            # a better way to do this
            time.sleep(delay)
    #after all tests have concluded clean up
    print_nonl("\n");
    logFile.write("\n");
    try:
        os.remove(os.path.join('.', testExec))
    except:
        pass
    os.remove(os.path.join('.', outName))
