import sys


valid = ['-v','-f','-c','-h','-d']
helpMSG = "\n===Refu Library Automated Testing Framework===\n\nValid parameters are:\n\n\
       -v:\tThe testing will be verbose and will output as many things as possible to the stdout\n\
       -f [FILENAME]:\tProvides the name of the lofgile to keep for all the tests\n\
       -c C1,C2,C3,...CN:\t Provide a list of valid compiler names separated by commas to compile and run the tests with. Make sure to provide no spaces in between\n\
       -d Test debugging. This options runs all the tests without actually comparing them to the expected output but instead spitting it all to the stdout. Used for testing the tests\n\
       -h:\tPrints this help message and quits the script"

class Args:
    testsLogName="TestsLog";
    verbose=False;
    compilers = ["GCC"];
    debug=False;

    def __init__(self):
        i = 0;
        skip=False;
        for arg in sys.argv[1:]:
            if(skip):
                skip=False;
                continue;
            if(arg in valid):
                if(arg == "-v"):
                    self.verbose = True;
                    print("Verbose is set")
                elif(arg == "-f"):
                    skip=True;
                    if(len(sys.argv)-1<i+1):#if no argument follows the -f
                        print("A filename should follow the -f argument, denoting the logfile to keep for all the tests")
                        sys.exit(0);
                    self.testsLogName = sys.argv[i+1];
                elif(arg == "-c"):
                    skip=True;
                    if(len(sys.argv)-1<i+1):#if no argument follows the -c
                        print("A comma separated list of valid compiler names must follow the -c argument")
                        sys.exit(0);
                    self.compilers = sys.argv[i+1].split(",");
                elif(arg == "-d"):
                    self.debug=True;
                elif(arg == "-h"):
                    print(helpMSG)
                    sys.exit(0)
            else:
                print("Provided illegal argument [{0}] to the Refu testing framework. Look at the help message below for the legal arguments\n".format(arg))
                print(helpMSG)
                sys.exit(0)

            i=i+1;

