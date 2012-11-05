import subprocess
from subprocess import CalledProcessError
import sys
import os


def checkConfigFile(root,configName,dynamic):
    """A function to check and edit the configuration file if needed

        --root: The path to the root directory of refu
        --configFName: The name of the configuration file
        --dynamic: A boolean flag denoting whether this is a dynamic build (True)
                   or a static build (False)
    """
    fname = os.path.join(root,configName);
    inF = open(fname,"r");
    outF = open(fname+"_temp","w");
    for line in inF:
        if(line.find("LIBRARY=")!=-1):
            if(dynamic):
                outF.write("LIBRARY=DYNAMIC\n");
            else:
                outF.write("LIBRARY=STATIC\n");
        else:
            outF.write(line);
    #after the copying is done cleanup
    inF.close();
    outF.close();
    os.remove(fname);
    os.rename(fname+"_temp",fname);


def compileLib(verbose,dynamic,config,compiler):
    """Compile the refu library by invoking the appropriate refu builder
        --verbose: A boolean flag denoting if the output should be verbose
        --dynamic: A boolean flag denoting whether a dynamic or a static library should be built
        --config: The configuration instance
        --compiler: The compiler instance
    """
    if(dynamic):
        print("--Compiling a dynamic version of the Refu library--")
    else:
        print("--Compiling a static version of the Refu library--")
    #make sure the refu builder will build correct library type
    checkConfigFile(config.refuDir,config.configFName,dynamic);
    #call the refu builder depending on the OS
    try:
        with subprocess.Popen([os.path.join(config.refuDir,config.builderFName), '-v', '-f', config.configFName],cwd="..",stdout=subprocess.PIPE) as p:
            if(verbose):
                print("\n\t==Builder Output Starts==\n")
                for line in p.stdout:
                    line = line.decode("utf8")
                    print("\t"+line,end="")
                print("\n\t==Builder Output Ends==\n")
            #wait until subprocess ends
            ret = p.poll();
            while(ret == None):
                ret = p.poll();
            if(ret!=0):
                print("\tRefu builder returned unsucessfully with return code {0}".format(ret))
                return False;
    except CalledProcessError as err:
        print("\tThere was an error while invoking the refu builder to compile the library")
        print("\tRefu builder returned [{0}] and the output was:".format(err.returncode))
        print(err.output);

    #if we get here it means success
    if(dynamic):
        print("--Dynamic version of the Refu library compiled Succesfully--\n\n");
        #move it inside the Tests directory
        try:
            os.rename(os.path.join(config.refuDir,config.outputName+compiler.dynamicExtension),os.path.join(config.refuDir,"Tests",config.outputName+compiler.dynamicExtension))
        except:
            pass
    else:
        print("--Static version of the Refu library compiled Succesfully--\n\n");
        #move it inside the Tests directory
        try:
            os.rename(os.path.join(config.refuDir,config.outputName+compiler.staticExtension),os.path.join(config.refuDir,"Tests",config.outputName+compiler.staticExtension))
        except:
            pass
    return True;