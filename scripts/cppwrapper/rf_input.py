#---------------------------------------------------------------------------------------#
# @author Lefteris                                                                      #
# @date 06/05/2012                                                                      #
#                                                                                       #
# This is a python script whose purpose is to parse a C file where there is a C         #
# structure defined in a specific way (using the Formatting                             #
# I am usually using in my projects) and attempts to spit out a header and source       #
# file that will contain a C++ wrapper of that very structure                           #
# To configure the input to this script open the input file which should be in the      #
# same directory as this script and by reading the comments of each input configure     #
# it properly.                                                                          #
# The name of the input file is "cppwrapper_input"                                      #
#---------------------------------------------------------------------------------------#




import sys

#! Just an exception that means that there was a parsing error
class ParserError(Exception):
    def __init__(self, value=""):
        self.value = value
    def __str__(self):
        return repr(self.value)

#! An exception that means we got a wrapping error
class WrapError(Exception):
    def __init__(self, value=""):
        self.value = value
    def __str__(self):
        return repr(self.value)

def printError(title,string):
    print("\n\n\n######PARSER ERROR - "+title+"######\n"+string+"\n####################################\n\n\n");
    raise WrapError;

def printWarning(title,string):
    print("\n\n\n######PARSER WARNING - "+title+"######\n"+string+"\n####################################\n\n\n");



#==============================Input file Parsing==============================#
#!This is the class that hold the input of the parser. All it does is read the input
# file and save it in an object of the class
class ParserInput:
    #input header filename
    inputFileName=""
    #output header file name
    outputHFileName=""
    #output source file name
    outputSFileName=""
    #name of the input struct
    structName=""
    #name of the class that will derive from the input struct
    className=""
    #the import export macro
    imex=""
    #namespace we will put the wrapper class in
    namespace=""
    #The starting signature of the functions that will be searched for in the input header file
    searchFuncSig=""
    #The starting signature of the function that will be searched for in the input header file and will operate as constructors
    ctorFuncSig=""
    #The starting signature of the function that will be ignored from the input header file
    ignoreFuncSig=""
    #A list of headers to include. Given from the input file
    headers=[];
    #The name of the structure that this structure will inherit from or "" for no inheritance
    parentStruct=""
    #The full name and path of the parent structure header file or "" for no inheritance
    parentStructFile=""
    #A list of all the ancestors of this struct starting with the oldest one
    ancestors = [];
    #a boolean flag denoting if the struct of the input is inherited by anything else
    isInherited = False;
    #A list of the children who inherit this structure
    children = [];
    #! The init function. It basically reads the input file
    #! @param verbose determines whether we output the progress of the parser to stdout or not
    def __init__(self,inputFileName,verbose):
        #open input file
        inputF = open(inputFileName,"r");
        #parse the input file
        for line in inputF:
            if(line[0]!='#'):#ignore comments. Comments can only be lines that START with #
                #get the desired input file name
                if(line.startswith("INPUT_FILE=")==True):
                    line=line.replace("INPUT_FILE=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    self.inputFileName = line;
                    if(verbose):
                        print("Input file is: "+inputFileName);
                #get the input struct name
                if(line.startswith("INPUT_STRUCT=")==True):
                    line=line.replace("INPUT_STRUCT=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    self.structName = line;
                    if(verbose):
                        print("Input struct is: "+self.structName);
                #get the desired output header file name
                if(line.startswith("OUTPUT_HEADER_FILE=")==True):
                    line=line.replace("OUTPUT_HEADER_FILE=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    self.outputHFileName = line;
                    if(verbose):
                        print("Output header file is: "+self.outputHFileName);
                #get the desired output source file name
                if(line.startswith("OUTPUT_SOURCE_FILE=")==True):
                    line=line.replace("OUTPUT_SOURCE_FILE=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    self.outputSFileName = line;
                    if(verbose):
                        print("Output header file is: "+self.outputSFileName);
                #get the desired namespace name
                if(line.startswith("NAMESPACE=")==True):
                    line=line.replace("NAMESPACE=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    self.namespace = line;
                    if(verbose):
                        print("Output namespace should be: "+self.namespace);
                #get the desired import export macro. What macro is used to export a function into a DLL/SO
                if(line.startswith("IMEX=")==True):
                    line=line.replace("IMEX=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    self.imex = line;
                    if(verbose):
                        print("Import export macro should be: "+self.imex);
                #get which header files should be included
                if(line.startswith("INCLUDED_HEADERS=")==True):
                    line=line.replace("INCLUDED_HEADERS=","");
                    line=line.rstrip('\n');
                    while True:
                        line=line.lstrip();
                        part = line.partition(",");
                        #if there is no comma left
                        if(part[1] == ""):
                            self.headers.append(line)
                            break;
                        #else
                        self.headers.append(part[0]);
                        line = part[2];

                    if(verbose):
                        print("Headers that should be included in the wrapper header file are:");
                        for i in self.headers:
                            print(i);
                #get the possible parent structure of this structure (if existing)
                if(line.startswith("INHERITS_STRUCT=")==True):
                    line=line.replace("INHERITS_STRUCT=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    self.parentStruct = line;
                    if(verbose):
                        if(self.parentStruct == ""):
                            print("According to the input file "+self.structName+" does not inherit from any other structure");
                        else:
                            print("According to the input file "+self.structName+" inherits from "+self.parentStruct);
                #get parent structure header (if existing)
                if(line.startswith("INHERITS_STRUCT_FILE=")==True):
                    line=line.replace("INHERITS_STRUCT_FILE=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    self.parentStructFile = line;
                    if(verbose):
                        if(self.parentStructFile == ""):
                            print("According to the input file "+self.structName+" does not inherit from any other structure");
                        else:
                            print("According to the input file "+self.structName+" inherits from a structure located at:\n"+self.parentStructFile);
                #get the ancestors of the struct if any exists
                if(line.startswith("ANCESTORS=")==True):
                    line=line.replace("ANCESTORS=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    #if we only have one ancestor or no ancestor
                    if(line.find(",") == -1):
                        line = line.rstrip();
                        #if we got no ancestor, put itself as the first and only ancestor
                        if(line == ""):
                            self.ancestors.append(self.structName);
                        #else put the only ancestor and then the struct
                        else:
                            self.ancestors.append(line);
                            self.ancestors.append(self.structName);
                    #else we got a comma separated ancestor chain
                    else:
                        self.ancestors = line.split(",");
                        #also add itself at the end of the chain
                        self.ancestors.append(self.structName);
                    #make sure that we got no trailing spaces at the struct names
                    for anc in self.ancestors:
                        anc = anc.lstrip();
                        anc = anc.rstrip();
                    if(verbose):
                        if(len(self.ancestors) == 0):
                            print("According to the input file "+self.structName+" does not inherit from any other structure");
                        else:
                            print("According to the input file "+self.structName+"'s base structure is: "+self.ancestors[0]);
                #get existence of further inheritance of the structure or not
                if(line.startswith("INHERITED_BY=")==True):
                    line=line.replace("INHERITED_BY=","");
                    line=line.lstrip();
                    line=line.rstrip('\n');
                    line=line.rstrip();

                    if(line.find(",") == -1):
                        line = line.rstrip();
                        #if we got no children
                        if(line == ""):
                            self.isInherited = False;
                        #else put the only child  in the list
                        else:
                            self.isInherited = True;
                            self.children.append(line);
                    #else we got a comma separated children list
                    else:
                        self.isInherited = True;
                        self.children = line.split(",");
                    #make sure that we got no trailing spaces at the struct names
                    for child in self.children:
                        child = child.lstrip();
                        child = child.rstrip();
                    if(verbose):
                        if(self.isInherited):
                            print("According to the input file "+self.structName+" is inherited by these structures");
                            for child in self.children:
                                print(child);
                        else:
                            print("According to the input file "+self.structName+" is not inherited by any other structures");

        #---compute some extra stuff---#
        #get the class name and check if it is separated by an underscore
        self.className= (self.structName.partition("_"))[2];
        if(self.className == ""):
            printError("ILLEGAL STRUCTURE NAME","The given Structure name \""+self.structName+"\" is invalid");
            sys.exit();
        #check if the parents are okay
        if(self.parentStruct != "" and self.parentStructFile == ""):
            printError("ILLEGAL STRUCTURE INHERITANCE","The given structure \""+self.structName+"\" inherits from \""+self.parentStruct+"\" but no header file is given for the parent structure");

        #create the search function signatures
        self.searchFuncSig = (self.structName.partition("_")[0]).lower();
        self.searchFuncSig += self.className+"_";
        #Create functions will be our copies of constructor
        self.ctorFuncSig = self.searchFuncSig+"Create";
        #Equivalent Init functions will be ignored
        self.ignoreFuncSig = self.searchFuncSig+"Init";
        inputF.close();#we are done parsing user input, close the file

    #! Clears the input object
    def clear(self):
        self.headers[:] = [];
        self.ancestors[:] = [];
        #self.children[:] = [];





