import subprocess
from subprocess import CalledProcessError
import sys
import os
import platform
import shlex

from output import TestCompileError, report

def compileTest(filename,dynamic,compiler,verbose,logFile):
    """Compiles a single test file from the tests depending on the compiler and library linking type

        --filename: The name of the file to compile without the 
        .c extension (to also match it with .expect)
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

    if(platform.system() == 'Windows'):
        sconsCall = ["scons.py", target,'-Q','COMPILER='+compiler,
                     '__TEST_SOURCE='+filename]
    else:
        sconsCall = "{} scons.py {} -Q COMPILER={} __TEST_SOURCE={}".format(
            sys.executable, target, compiler, filename)

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
            report(logFile, "\t\tDuring compiling {0} the {1} compiler returned "
                   "unsucessfully with return code {2}".format(
                       filename, compiler, ret))
            msg = ""
            if p.stdout is not None:
                for line in p.stdout:
                    line = line.decode("utf8")
                    msg = msg + line
                    report(logFile, "\t\t"+line)
            raise TestCompileError(filename, msg)
                
    except CalledProcessError as err:
        report(logFile, "\t\tThere was an error while invoking the compiler for"
              " test \"{0}\"".format(filename))
        report(logfile, "\t\tThe compiler returned [{0}] and the output "
              "was:".format(err.returncode))
        raise TestCompileError(filename, err.output)
    #success
    if(verbose):
        print("\t-> Succesfully compiled Test \"{0}\"".format(filename));
