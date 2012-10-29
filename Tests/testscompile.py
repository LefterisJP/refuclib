import subprocess
from subprocess import CalledProcessError
import sys
import os
import shlex

from compiler import Compiler
from configreader import Config


def compileTest(filename,dynamic,config,compiler,testExec,verbose,logFile):
    compilerFullPath = os.path.join(config.compilerDir,compiler.compilerExec)
    cmd = compiler.cmd;
    cmd = cmd.replace("$REFUINCLUDE",os.path.join(config.refuDir,"include"));
    cmd = cmd.replace("$FLAGS",compiler.flags);
    cmd = cmd.replace("$LIBDIR",config.refuDir);
    if(dynamic):
        cmd = cmd.replace("$LIBNAME",os.path.join(config.refuDir,config.outputName+compiler.dynamicExtension))
    else:#static
        cmd = cmd.replace("$LIBNAME",os.path.join(config.refuDir,config.outputName+compiler.staticExtension))
    if(config.system == "win32"):
        cmd = cmd.replace("$OUTPUT","test.exe")
    else:
	    cmd = cmd.replace("$OUTPUT","test")
    cmd = cmd.replace("$FILE",filename)
    cmd = compilerFullPath+" "+cmd;
    #for unix-like systems we can use the shlex.split() function to get a list of arguments
    if(config.system == "linux"):
        cmdList = shlex.split(cmd)
    else:#for windows just provide the whole string
        cmdList = cmd;
    try:
        with subprocess.Popen(cmdList,stdout=subprocess.PIPE,stderr=subprocess.PIPE) as p:
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
                return False;
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
