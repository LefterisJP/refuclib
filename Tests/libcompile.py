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
        --dynamic: A boolean flag denoting whether a dynamic or a static
                   library should be built
        --compiler: The compiler name. Can be one of ['gcc', 'tcc', 'msvc']
        
        Returns the output name produced by compiling the library
    """
    #first of all clean the related directories
    cleanDir(".")

    if dynamic:
        print("--Compiling a dynamic version of the Refu library--")
        arg = "shared"
    else:
        print("--Compiling a static version of the Refu library--")
        arg = "static"
    
    # for now we give here all the options to generate extra sources that
    # all the tests need. In the future if the library compiles again for
    # each module tested this will need to change. And as for the extra
    # objects, omg look at all these escape characters >.<
    extra_source_opts = (
        " EXTRA_OBJECTS=[[\\\"test_object\\\",\\\"test_obj\\\",\\\"Tests/ExtraObjects/test.c\\\",\\\"Tests/ExtraObjects/test.h\\\",[\\\"test_destroy\\\",\\\"test_copy\\\",\\\"test_equal\\\",[]]]]"
        " LIST=I,String,test_object DYNAMIC_ARRAY=I,String "
        "HASHMAP=I,String "
    )

    if platform.system() == 'Windows':
        sconsCall = ['scons.py', arg, 'COMPILER='+compiler,
                     extra_source_opts]
    else:
        sconsCall = "{} scons.py {} COMPILER={} {}".format(
            sys.executable, arg, compiler, extra_source_opts)

    #call scons to build the library
    try:
        p = subprocess.Popen(sconsCall, cwd="..", shell=True,
                             stdout=subprocess.PIPE)
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
            print("\tScons returned unsucessfully with return code "
                  "{}. Scons output follows:\n".format(ret))
            for line in p.stdout:
                line = line.decode("utf8")
                sys.stdout.write("\t"+line)
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
