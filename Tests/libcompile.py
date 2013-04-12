import subprocess
from subprocess import CalledProcessError
import sys
import os
import platform


def cleanDir(target):
	"""Cleans the target dir of any binaries"""
	for item in os.listdir(target):
		if os.path.isfile(os.path.join(".", item)):
			if item.endswith(('.a', '.dll', '.lib','so')):
				os.remove(os.path.join(".",item))	
				

def findOutput(dynamic):
	"""Find the output suffix of the compiled library binary"""
	#find the output type
	type = ""
	if(platform.system() == 'Windows'):
		if(dynamic):
			type = ".dll"
		else:
			type = ('.lib','.a')
	else:
		if(dynamic):
			type = ".so"
		else:
			type = ('lib','.a')
	#search for that type
	for item in os.listdir(".."):
		if os.path.isfile(os.path.join("..", item)):
			if item.endswith(type):
				return item;


def compileLib(verbose,dynamic,compiler):
    """Compile the refu library by invoking the appropriate refu builder
        --verbose: A boolean flag denoting if the output should be verbose
        --dynamic: A boolean flag denoting whether a dynamic or a static library should be built
        --compiler: The compiler name. Can be one of ['gcc', 'tcc', 'msvc']
        
        Returns the output name produced by compiling the library
    """
    #first of all clean the related directories
    cleanDir(".")

    if(dynamic):
        print("--Compiling a dynamic version of the Refu library--")
        arg="shared"
    else:
        print("--Compiling a static version of the Refu library--")
        arg="static"

    if(platform.system()=='Windows'):
        sconsCall = ['scons.py',arg,'--compiler='+compiler]
    else:
        sconsCall = "python scons.py "+arg+" --compiler="+compiler
    #call scons to build the library
    try:
        #p = subprocess.Popen([sconsCall, arg,'--compiler='+compiler],cwd="..",shell=True,stdout=subprocess.PIPE)
        p = subprocess.Popen(sconsCall,cwd="..",shell=True,stdout=subprocess.PIPE)
        if(verbose):
            print("\n\t==Scons Output Starts==\n")
            for line in p.stdout:
                line = line.decode("utf8")
                sys.stdout.write("\t"+line)
            print("\n\t==Scons Output Ends==\n")
        #wait until subprocess ends
        ret = p.poll();
        while(ret == None):
            ret = p.poll();
        if(ret!=0):
            print("\tScons returned unsucessfully with return code {0}".format(ret))
            return False;
    except CalledProcessError as err:
        print("\tThere was an error while invoking scons to compile the library")
        print("\tScons returned [{0}] and the output was:".format(err.returncode))
        print(err.output);

    #now find the output name
    outName = findOutput(dynamic)
    #if we get here it means success
    if(dynamic):
        print("--Dynamic version of the Refu library compiled Succesfully--\n\n");
    else:
        print("--Static version of the Refu library compiled Succesfully--\n\n");
    #move the compiled binaries into the tests dir
    try:
        os.rename(os.path.join("..",outName),os.path.join("..","Tests",outName))
    except:
        pass

    return outName;
