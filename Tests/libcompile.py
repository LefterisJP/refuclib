import subprocess
from subprocess import CalledProcessError
from output import print_nonl
import os
import sys
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


def compileLib(verbose, dynamic, compiler):
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
        print_nonl("==> Compiling a dynamic version of the Refu library...")
        arg = "shared"
    else:
        print_nonl("==> Compiling a static version of the Refu library...")
        arg = "static"
    
    # for now we give here all the options to generate extra sources that
    # all the tests need. In the future if the library compiles again for
    # each module tested this will need to change.
    extra_source_opts = (
        " EXTRA_OBJECTS=\"[{name:test_object,"
        "source_name:test_obj,"
        "headers:[../ExtraObjects/test.h],"
        "destroy_func:test_destroy,"
        "copy_func:test_copy,"
        "compare_func:test_equal"
        "}]\""
        " LIST=I,String,test_object DYNAMIC_ARRAY=I,String "
        "HASHMAP=I,String,generic "
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
                print_nonl("\t"+line)
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
                print_nonl("\t"+line)
            return False;
    except CalledProcessError as err:
        print("\tThere was an error while invoking scons to compile the library")
        print("\tScons returned [{0}] and the output was:".format(err.returncode))
        print(err.output);

    #now find the output name
    outName = findOutput(dynamic)
    #if we get here it means success
    if verbose:
        print("*** Library compiling was successfull ***")
    else:
        print("SUCCESS!\n")
    #move the compiled binaries into the tests dir
    try:
        os.rename(os.path.join("..",outName),os.path.join("..","Tests",outName))
    except:
        pass

    return outName;
