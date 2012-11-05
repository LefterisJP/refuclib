import subprocess
from subprocess import CalledProcessError
import os, os.path
from testscompile import compileTest
import shutil

def test(root,filename,config,logFile,testExec,verbose):
    """
        Tests a compiled test executable and compared it to the expected output
        Arguments:

        --root: The root directory of the test compared to the Tests directory
        --filename: The filename of the test
        --config: The configuration instance
        --logFile: The name of the logfile
        --testExec: The name of the test executable
        --veborse: A boolean flag denoting if the tests should be verbose or not
    """
    print("\t--> Testing \"{0}\"...".format(filename),end="")
    logFile.write("\t--> Testing \"{0}\"...".format(filename))
    #open the expected outputfile
    f = open(filename+".expect","r")
    try:
        with subprocess.Popen(os.path.join(config.refuDir,"Tests",os.path.split(root)[1],testExec),cwd=root,stdout=subprocess.PIPE) as p:
                count = 1;
                for line in p.stdout:#for every line of the output
                    line = line.decode("utf8");
                    line = line.replace("\r\n","\n")
                    expectedLine = f.readline();
                    if(line!=expectedLine):
                        if("*ERROR*:" in line):#it it's an expect error
                            print("FAILED!\n\t"+line);
                            logFile.write("FAILED!\n\t"+line);
                        else:
                            print("FAILED!\n\tCheck the logfile for Details.")
                        #print("\t(Check logFile for details)\n")
                        logFile.write("\n\t\tTest failed at expected output line \"[{0}]\"\n".format(count))
                        logFile.write("\n\t\tExpected:\n\t\t\t{0}\n\t\tFound:\n\t\t\t{1}\n".format(expectedLine,line))
                        return False;
                    count=count+1
                #wait till the subprocess ends
                ret = p.poll();
                while(ret == None):
                    ret=p.poll();
    except CalledProcessError as err:
        print("\tThere was an error while trying to run the compiled executable for test \"{0}\"".format(filename))
        print("\tThe output was:")
        print(err.output);
        logFile.write("\tTest \"{0}\" failed due to inability to run the executable\n".format(filename))
        return False;
    #success
    if(ret == 0):
        print("OK!\n")
        logFile.write("OK!\n")
    else:
        print("FAILED! Unexpected Return Value.\n")
        logFile.write("FAILED! Unexpected Return Value.\n")
    return True;

def debugTest(root,filename,config,logFile,testExec):
    """
        Debugs a compiled test executable by just spitting out its output
        --root: The root directory of the test compared to the Tests directory
        --filename: The filename of the test
        --config: The configuration instance
        --logFile: The name of the logfile
        --testExec: The name of the test executable
    """
    print("\t**Testing \"{0}\" in Debug Mode**\n".format(filename))
    logFile.write("\t**Testing \"{0}\" in Debug Mode**\n".format(filename))
    try:
        with subprocess.Popen(os.path.join(config.refuDir,"Tests",os.path.split(root)[1],testExec),cwd=root,stdout=subprocess.PIPE) as p:
                for line in p.stdout:#for every line of the output
                    line = line.decode("utf8");
                    line = line.replace("\r\n","\n")
                    print("\t"+line,end="")
                    logFile.write(line)
                #wait till the subprocess ends
                ret = p.poll();
                while(ret == None):
                    ret=p.poll();
    except CalledProcessError as err:
        print("\tThere was an error while trying to run the compiled executable for test \"{0}\" in debug mode".format(filename))
        print("\tThe output was:")
        print(err.output);
        logFile.write("\tTest \"{0}\" failed due to inability to run the executable\n".format(filename))
        return False;

def runTests(config,compiler,dynamic,logFile,verbose,debug,tests):
    """Runs the Tests of the Refu library

        --config: The instance of the system's configuration read in by parsing
                  the configuration filefa
        --compiler: The instance of the compilers
        --dynamic: A boolean flag, true for using a dynamic version of the library
                  and false for static
        --logFile: The log file to keep so that the user can see a
                overview of how the tests performed.
        --verbose: A boolean flag denoting whether the function should be verbose
                    about its actions or not
        --debug: A boolean flag denoting whether the tests should be in debug mode
                 (no testing or comparison) or not
        --tests: A list of specific tests to run. If it is empty all tests are run. If it is not
                then only the specified ones are.
    """
    if(dynamic):
        logFile.write("---Starting Tests for the Dynamic version of Refu with the {0} Compiler---\n\n".format(compiler.name))
        print("--Starting Tests for the Dynamic version of Refu with the {0} Compiler---\n".format(compiler.name))
    else:
        logFile.write("---Starting Tests for the Static version of Refu with {0} Compiler---\n\n".format(compiler.name))
        print("---Starting Tests for the Static version of Refu with {0} Compiler---\n".format(compiler.name))
    if(config.system=="win32"):
        testExec = "test.exe"
    else:
        testExec = "test"
    #Recursively iterate all the files in the Tests directory
    for root, dirs, files in os.walk("./"):
        for f in files:
            fullname = os.path.join(root,f);
            #if it's a test get it's equivalent .c file
            if(fullname.endswith(".expect")):
                part = fullname.partition(".expect")[0];
                #check to see if the user provided specific tests, and if he did check to see if this is one of them
                if(tests):
                    if(os.path.split(part)[1] not in tests):
                        continue;
                #compile it
                if(compileTest(part+".c",dynamic,config,compiler,testExec,verbose,logFile)==False):
                    continue;#if it fails go to the next test
                #move the executable to the specific test directory its source was and if there was one there already delete it
                try:
                    os.remove(os.path.join(root,testExec))
                except:
                    pass
                os.rename(os.path.join(".",testExec),os.path.join(root,testExec))
                if(dynamic):#also if we are testing the dynamic lib, copy it to the test directory source so that the lib can be accessed
                    shutil.copy(os.path.join(config.refuDir,"Tests",config.outputName+compiler.dynamicExtension),os.path.join(root,config.outputName+compiler.dynamicExtension))
                #test it
                if(debug):
                    debugTest(root,part,config,logFile,testExec)
                else:
                    test(root,part,config,logFile,testExec,verbose)
                if(dynamic):#also if we are testing the dynamic lib, delete its local copy
                    os.remove(os.path.join(root,config.outputName+compiler.dynamicExtension))
                #after finishing a test delete the executable
                try:
                    os.remove(os.path.join(root,testExec))
                except:
                    pass
    #after all tests have concluded clean up
    print("\n",end="");
    logFile.write("\n");
    try:
        os.remove(os.path.join(config.refuDir,testExec))
    except:
        pass
    if(dynamic):
        os.remove(os.path.join(config.refuDir,"Tests",config.outputName+compiler.dynamicExtension))
    else:
        os.remove(os.path.join(config.refuDir,"Tests",config.outputName+compiler.staticExtension))
