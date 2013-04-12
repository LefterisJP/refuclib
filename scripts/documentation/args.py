import sys
from ProjectPaths import doxyconfigfile

valid = ['-p','-v','-h']
helpMSG = "\n===Refu Library Documentation Generator===\n\nValid parameters are:\n\n\
       -v:\tVerbose output\n\
       -p:\tThe private doxygen configuration file is used\n\
       -h:\tPrints this help message and quits the script"

doxyconfigfilePrivate = "C:\\Projects_SourceCode\\ReFu\\docs\\refu_doxyfile_temp"

class Args:
    private=False;
    verbose=False;
    configFile=doxyconfigfile
    def __init__(self):
        for arg in sys.argv[1:]:
            if(arg in valid):
                if(arg == "-v"):
                    self.verbose = True;
                    print("Verbose is set")
                elif(arg == "-p"):
                    self.private = True;
                    self.configFile = changeConfig()
                    print("The private doxygen configuration file will be used\n");
                elif(arg == "-h"):
                    print(helpMSG)
                    sys.exit(0)
            else:
                print("Provided illegal argument [{0}] to the Refu library's Documentation gGenerator\n".format(arg))
                print(helpMSG)
                sys.exit(0)

#A function to create a temporary private doxygen configuration file, by making a copy of the normal one and adding specific changes to it
def changeConfig():
    inF = open(doxyconfigfile,"r",encoding="utf8")
    outF=open(doxyconfigfile+"_temp","w",encoding="utf8");
    #iterate all the lines of the input file making changes where necessary
    for line in inF:
        if(line.find("#")==0):#if it's a comment, just copy the line
            outF.write(line);
            continue;

        if("SHOW_FILES" in line):#in these special cases turn them to yes
            outF.write("SHOW_FILES = YES\n")
            continue;
        elif("EXTRACT_ALL" in line):
            outF.write("EXTRACT_ALL = YES\n")
            continue;
        elif("INCLUDE_GRAPH" in line):
            outF.write("INCLUDE_GRAPH = YES\n")
            continue;
        elif("EXTRACT_PRIVATE" in line):
            outF.write("EXTRACT_PRIVATE = YES\n")
            continue;
        elif("EXTRACT_STATIC" in line):
            outF.write("EXTRACT_STATIC = YES\n")
            continue;
        #in al other cases simply copy the line
        outF.write(line)
    inF.close();
    outF.close();
    return doxyconfigfile+"_temp";


