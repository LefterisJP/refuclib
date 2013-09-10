import subprocess
from subprocess import CalledProcessError
import sys
import os
import platform
import shlex

from output import TestCompileError, report

def compileTest(test_name, test_sources, dynamic, compiler, verbose, logFile):
    """Compiles a single test file from the tests depending on the compiler and library linking type

        --test_name: The name of the test. This also matches the .expect file 
        if existing
        --test_sources: A list of test sources to compile
        --dynamic: A boolean value denoting if the linking should be done
        with a dynamic/shared library or not (static)
        --compiler: The name of the compiler with which to do the compiling
        --verbose: A boolean value denoting if the output to stdout
        should be verbose or not
        --logFile: The name of the logfile to print the output
    """
    if(dynamic):
        target="test_shared"
    else:
        target="test_static"

    sources_string = "__TEST_SOURCES="
    for s in test_sources:
        sources_string = sources_string+s+","
    sources_string = sources_string[:-1]

    if(platform.system() == 'Windows'):
        sconsCall = ["scons.py", target,'-Q','COMPILER='+compiler, sources_string]
    else:
        sconsCall = "{} scons.py {} -Q COMPILER={} {}".format(
            sys.executable, target, compiler, sources_string)

    try:
        p = subprocess.Popen(sconsCall, cwd="..", shell=True, stdout=subprocess.PIPE)
        for line in p.stdout:
            if(verbose):
                print("\t\t"+line.decode("utf8"));
        #wait till the subprocess ends
        ret = p.poll();
        while(ret == None):
            ret = p.poll()
        if ret != 0:
            report(logFile, "\t\tDuring compiling test {0} the {1} compiler returned "
                   "unsucessfully with return code {2}".format(
                       test_name, compiler, ret))
            msg = ""
            if p.stdout is not None:
                for line in p.stdout:
                    line = line.decode("utf8")
                    msg = msg + line
                    report(logFile, "\t\t"+line)
            raise TestCompileError(test_name, msg)
                
    except CalledProcessError as err:
        report(logFile, "\t\tThere was an error while invoking the compiler for"
              " test \"{0}\"".format(test_name))
        report(logfile, "\t\tThe compiler returned [{0}] and the output "
              "was:".format(err.returncode))
        raise TestCompileError(test_name, err.output)
    #success
    if(verbose):
        print("\t-> Succesfully compiled Test \"{0}\"".format(test_name));
