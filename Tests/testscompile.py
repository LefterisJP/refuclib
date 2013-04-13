import subprocess
from subprocess import CalledProcessError
import sys
import os
import platform
import shlex

def compileTest(filename,dynamic,compiler,verbose,logFile):
    """Compiles a single test file from the tests depending on the compiler and library linking type

        --filename: The name of the file to compile without the .c extension (to also match it with .expect)
        --dynamic: A boolean value denoting if the linking should be done with a dynamic/shared library or not (static)
        --compiler: The name of the compiler with which to do the compiling
        --verbose: A boolean value denoting if the output to stdout should be verbose or not
        --logFile: The name of the logfile to print the output
    """
    if(dynamic):
        target="test_shared";
    else:
        target="test_static";

    if(platform.system() == 'Windows'):
        sconsCall = ["scons.py", target,'-Q','COMPILER='+compiler,'__TEST_SOURCE='+filename]
    else:
        sconsCall = "python scons.py "+target+" -Q COMPILER="+compiler+" __TEST_SOURCE="+filename

    try:
        p = subprocess.Popen(sconsCall,cwd="..",shell=True,stdout=subprocess.PIPE)
        for line in p.stdout:
            if(verbose):
                print("\t\t"+line.decode("utf8"));
        #wait till the subprocess ends
        ret = p.poll();
        while(ret == None):
            ret=p.poll();
        if(ret!=0):
            if(verbose):
                print("\t\tDuring compiling {0} the {1} compiler returned unsucessfully with return code {2}".format(filename,compiler.name,ret))
            logFile.write("\t\tTesting of file \"{0}\" failed in the compilation stage\n".format(filename))
            for line in p.stderr:
                line = line.decode("utf8");
                print("\t\t"+line)
                logFile.write("\t\t"+line);
            print("\t-->Testing \"{0}\" ... FAILED to compile".format(filename))
            logFile.write("\t-->Testing \"{0}\" ... FAILED to compile\n".format(filename))
            return False
    except CalledProcessError as err:
        print("\t\tThere was an error while invoking the compiler for test \"{0}\"".format(filename))
        print("\t\tThe compiler returned [{0}] and the output was:".format(err.returncode))
        print(err.output);
        logFile.write("\t\tTesting of file \"{0}\" failed in the compilation stage\n".format(filename))
        return False;
    #success
    if(verbose):
        print("\t-> Succesfully compiled Test \"{0}\"".format(filename));
    return True;
