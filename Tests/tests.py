import subprocess
from subprocess import CalledProcessError
import os, os.path
from testscompile import compileTest
import shutil
import time
import platform
import sys

#the time by which to delay actions after compiling so as not to have conflicts
#after copying/deleting binaries
delay = 0.20

def configureLinking(dynamic,root,outName):
    """Deals with the library linking for each test"""
    if(dynamic):
        #if we are testing the dynamic lib in windows copy it to the
        #test  directory source so that the lib can be accessed
        if(platform.system()=='Windows'):
            try:
                os.remove(os.path.join(root,outName))
            except:
                pass
            shutil.copy(os.path.join(".",outName),os.path.join(root,outName))
   

def test(root,filename,logFile,testExec,verbose):
    """
        Tests a compiled test executable and compared it to the expected output
        Arguments:

        --root: The root directory of the test compared to the Tests directory
        --filename: The filename of the test
        --logFile: The name of the logfile
        --testExec: The name of the test executable
        --verbose: A boolean flag denoting if the tests should be verbose or not
    """
    #Remove Tests/ part of the directory from the filename
    tempName = os.path.split(filename)
    filename = os.path.join(os.path.split(tempName[0])[1] , tempName[1])
    sys.stdout.write("\t--> Testing \""+filename+"\"...")
    logFile.write("\t--> Testing \""+filename+"\"...")
    #open the expected outputfile
    f = open(filename+'.expect','r')
    try:
        #p = subprocess.Popen(os.path.join(os.path.split(root)[1],testExec),cwd=root,stdout=subprocess.PIPE)
        p = subprocess.Popen(os.path.join('.',testExec),cwd=root,stdout=subprocess.PIPE)
        count = 1;
        for line in p.stdout:#for every line of the output
            line = line.decode("utf8");
            line = line.replace("\r\n","\n")
            if("passed >>" in line):
                logFile.write("\t\t\t"+line+"\n");
                continue;
            expectedLine = f.readline();
            if(line!=expectedLine):
                if("*ERROR*:" in line):#it it's an expect error
                    print("FAILED!\n\t"+line);
                    logFile.write("FAILED!\n\t"+line);
                else:
                    print("FAILED!\n\tCheck the logfile for Details.")
                    logFile.write("\n\t\tTest failed at expected output line \"["+str(count)+"]\"\n")
                    logFile.write("\n\t\tExpected:\n\t\t\t"+str(expectedLine)+"\n\t\tFound:\n\t\t\t"+str(line)+"\n")
                    return False;
                count=count+1
        #wait till the subprocess ends
        ret = p.poll();
        while(ret == None):
            ret=p.poll();
    except CalledProcessError as err:
        print("\tThere was an error while trying to run the compiled executable for test \""+filename+"\"")
        print("\tThe output was:")
        print(err.output);
        logFile.write("\tTest \""+filename+"\" failed due to inability to run the executable\n")
        return False;
    #success
    if(ret == 0):
        print("OK!\n")
        logFile.write("OK!\n")
    else:
        print("FAILED!\n\t\t Unexpected Return Value. Check the logfile for details")
        line = line.partition("[")[2];
        line = line.partition("]")[0];
        print("\t\tTest ran until line ["+str(line)+"]\n")
        logFile.write("\t\tTEST FAILED! Unexpected Return Value.\n")
    return True;

def debugTest(root,filename,logFile,testExec):
    """
        Debugs a compiled test executable by just spitting out its output
        --root: The root directory of the test compared to the Tests directory
        --filename: The filename of the test
        --logFile: The name of the logfile
        --testExec: The name of the test executable
    """
    #Remove Tests/ part of the directory from the filename
    tempName = os.path.split(filename)
    filename = os.path.join(os.path.split(tempName[0])[1] , tempName[1])

    print("\t**Testing \""+filename+"\" in Debug Mode**\n")
    logFile.write("\t**Testing \""+filename+"\" in Debug Mode**\n")
    try:
        #p = subprocess.Popen(os.path.join("Tests",os.path.split(root)[1],testExec),cwd=root,stdout=subprocess.PIPE)
        p = subprocess.Popen(os.path.join('.',testExec),cwd=root,stdout=subprocess.PIPE)
        for line in p.stdout:#for every line of the output
            line = line.decode("utf8");
            line = line.replace("\r\n","\n")
            sys.stdout.write("\t"+line)
            logFile.write(line)
        #wait till the subprocess ends
        ret = p.poll();
        while(ret == None):
            ret=p.poll();
    except CalledProcessError as err:
        print("\tThere was an error while trying to run the compiled executable for test \""+filename+"\" in debug mode")
        print("\tThe output was:")
        print(err.output);
        logFile.write("\tTest \""+filename+"\" failed due to inability to run the executable\n")
        return False;

def runTests(compiler,dynamic,outName,logFile,verbose,debug,tests):
    """Runs the Tests of the Refu library

        --compiler: The name of the compiler
        --dynamic: A boolean flag, true for using a dynamic version of the library and false for static
        --outName: The full filename of the compiled refu library with which to link
        --logFile: The log file to keep so that the user can see an overview of how the tests are performed.
        --verbose: A boolean flag denoting whether the function should be verbose about its actions or not
        --debug: A boolean flag denoting whether the tests should be in debug mode (no testing or comparison) or not
        --tests: A list of specific tests to run. If it is empty all tests are run. If it is not then only the specified ones are.
    """
    #test executable name is always test for now
    if(platform.system() == 'Windows'):
        testExec = 'test.exe'
    else:
        testExec = 'test'
    if(dynamic):
        logFile.write("---Starting Tests for the Dynamic version of Refu with the \""+compiler+"\" Compiler---\n\n")
        print("--Starting Tests for the Dynamic version of Refu with the \""+compiler+"\" Compiler---\n")
    else:
        logFile.write("---Starting Tests for the Static version of Refu with \""+compiler+"\" Compiler---\n\n")
        print("---Starting Tests for the Static version of Refu with \""+compiler+"\" Compiler---\n")
    #clean the tests directory from object files
    for root, dirs, files in os.walk("."):
        for f in files:
            if(f.endswith(('.o','.obj'))):
                os.remove(os.path.join(root,f))
    #Recursively iterate all the files in the Tests directory
    for root, dirs, files in os.walk("."):
        for f in files:
            fullname = os.path.join(root,f);
            fullname = fullname.replace('.','Tests',1)
            #if it's a test get it's equivalent .c file
            if(fullname.endswith(".expect")):
                part = fullname.partition(".expect")[0];
                #check to see if the user provided specific tests, and if he did check to see if this is one of them
                if(tests):
                    if(os.path.split(part)[1] not in tests):
                        continue;
                #compile it
                if(compileTest(part+".c",dynamic,compiler,verbose,logFile)==False):
                    continue;#if it fails go to the next test
                #after each compile add a small delay since the subsequent copy may fail
                time.sleep(delay)
                #move the executable to the specific test directory its source was and if there was one there already delete it
                try:
                    os.remove(os.path.join(root,testExec))
                except:
                    pass
                os.rename(os.path.join(".",testExec),os.path.join(root,testExec))
                #deal with the library linking for each test
                configureLinking(dynamic,root,outName)
                #test it
                if(debug):
                    debugTest(root,part,logFile,testExec)
                else:
                    test(root,part,logFile,testExec,verbose)
                if(dynamic):#also if we are testing the dynamic lib, delete its local copy
                    try:
                        os.remove(os.path.join(root,outName))
                    except:
                        pass
                    #after finishing a test delete the executable
                    try:
                        os.remove(os.path.join(root,testExec))
                    except:
                        pass
                    #adding some delay between tests until I figure out a better way to do this
                    time.sleep(delay)
    #after all tests have concluded clean up
    sys.stdout.write("\n");
    logFile.write("\n");
    try:
        os.remove(os.path.join('.',testExec))
    except:
        pass
    os.remove(os.path.join('.',outName))
