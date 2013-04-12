#---------------------------------------------------------------------------------------#
# @author Lefteris                                                                      #
# @date 08/05/2012                                                                      #
#                                                                                       #
# This file contains the c function class
# which basically is a container for keeping a c function for the parser                #
#---------------------------------------------------------------------------------------#

import sys

from rf_input import printError,printWarning


#!Just a class to "emulate" enum. Saw that in stack overflow
class PARAMS:
    NORMAL      =1
    CLASSOBJECT =2
    KEEPPTR     =3
    GIVENTYPE   =4

#!Just a class to "emulate" enum. Saw that in stack overflow
class OPERATORS:
    NONE            =1
    ASSIGNMENT      =2
    ADDITION        =3
    COMPARISONEQ    =4


#! Just an exception that means that a constructor failed
class CtorError(Exception):
    def __init__(self, value=""):
        self.value = value
    def __str__(self):
        return repr(self.value)



#! Checks a parameter flag list for a specific flag of a specific parameter
#! @param The list of flag to search in
#! @param i The index of the parameter to check for
#! @param flag The flag to search for in the parameters flags of this parameter
#! @return Returns the value of the requested flag or -1 for failure
def checkPFlag(flagsList,i,flag):
    #get the list of flags for the i parameter
    if(i > len(flagsList)):
       printError("PARAM FLAGS CHECKING","During checking the parameters flag in a list the given index '"+str(i)+"' is out of bounds");
       return -1;
    pList = flagsList[i];
    #print("Searching for flag "+str(flag));
    #search all the flags to see if the required flag is there and return its value
    for paramFlag in pList:
       #print("Comparing with flag:"+str(paramFlag));
       if(paramFlag[0] == flag):
            #print("Found!");
            #return the value
            return paramFlag[1];
    #if we get here it means it was not found
    #print("NOT Found");
    return -1;

def stringFindList(thisstr,alist):
    for astr in alist:
        #print("Comparing \""+thisstr+"\" with \""+astr+"\"");
        if(thisstr == astr):
            return astr;
    return "";

# Searches this string for the existence of any ancestor string from the ancestors List
# The function searches for both the ancestor structures and the ancestor classes derivin from the list
#! @param thisstr The string to search in
#! @param ancestorL A list of ancestor STRUCTURES
#! @return Returns a triple with the first part of it being the struct name of the ancestor found  and the second being 1 for class and 0 for structure and the third containing the class name
#! If nothing is found it returns : ["",-1,""]
def hasAncestor(thisstr,ancestorsL):
    #first and foremost make sure that the parameter is reduced to its most basic type
    thisstr = thisstr.replace("&","");
    thisstr = thisstr.replace("*","");
    thisstr = thisstr.replace("const","");
    thisstr = thisstr.replace("static","");
    thisstr = thisstr.rstrip();
    thisstr = thisstr.lstrip();
    #try to find the normal ancestors structures inside
    structRes = stringFindList(thisstr,ancestorsL);
    #if no ancestor structure name is found
    if(structRes == ""):
        #create the list of ancestor classes and search for any of them
        classNames= [];
        for astr in ancestorsL:
            classNames.append(astr.partition("_")[2]);
        classRes = stringFindList(thisstr,classNames);
        classNames[:] = [];
        if(classRes == ""):
            return ["",-1,""];

        #in order to also get the struct name search back in the ancestorsL
        sName = ""
        for anc in ancestorsL:
            if(anc.partition("_")[2] == classRes):
                sName = anc;
        return [sName,1,classRes];
    else:
        return [structRes,0,structRes.partition("_")[2]];


#! A class to hold information about the parameters of a function
class FunctionParameters:
    #! The type of each parameter variable
    types = [];
    #! The value of each parameter variable
    values = [];
    #! Special flags for each parameter variable. Parsed from the comment block in the C code
    flags = [];
    #! The number of parameters
    number = 0;
    #! The parameters string
    pString = ""
    #A flag denoting if the function has a var arg parameter
    hasVarArgs = False;
    #! Initializes a function's parameters
    #! @param pString The parameter's string from which to create a clean and nice parameter string
    #! @param losefirst True if we wanna lose the first param since it's the this pointer. Will almost always be true
    #! @param structName The name of the structure whose function where the parameters were in
    #! @param className The name of the class that the function of the parameters shold be created in
    #! @param paramFlags A list of flags of parameters parsed from the comments above the function
    #! @param lineN The line number of the function. For debugging purposes
    #! @param funcName The name of the function. For debugging purposes
    #! @return Saves all the parameter data and returns a new changed parameters string
    def __init__(self,pString,losefirst,structName,className,paramFlags,lineN,funcName,comesfromdefine,ancestorsL):
        self.values[:] = [];
        self.types[:] = [];
        self.flags = paramFlags;
        if(losefirst == True):
            #lose the first parameter since it's the this pointer
            part = pString.partition(",");
            #print("Partitioning Gives: \nPart[0]:\""+part[0]+"\nPart[1]:\"\""+part[1]+"\"\nPart[2]:\""+part[2]+"\"");
            #if there is more than 1 param
            if(part[1] != ""):
                self.pString = part[2];
            #if there is only 1 param, then it's the this pointer and we need to get rid of it
            else:
                self.pString = "";
                self.number = 0;
                #SUCCESS
                return;
        else:
            self.pString = pString;

        #--parse the parameters one by one and mine all the available information--#
        #if it comes from a macro
        if(comesfromdefine == True):
            #get the pstring of the macro and the actual number of parameters
            self.pString = self.pString.partition(")")[0];
            tempCount = self.pString.count(",");
            if(tempCount == 0):
                self.number = 1;
            else:
                self.number = tempCount+1;
            #the macro comment block must have define the cpp types of all the parameters
            if(len(paramFlags) != self.number):
               printError("MACRO PARAMETERS ERROR", "The parameters for macro \""+funcName+"\" defined at line '"+str(lineN)+" have not been defined with a corresponding @cpptype. All parameters need to have this tag so that the wrapper knows what to do");
               return;

            #get type and value of each parameter
            tempPString = self.pString;
            for i in range(len(paramFlags)):
               pType = checkPFlag(paramFlags,i,PARAMS.GIVENTYPE);
               if(pType == -1):
                   printError("MACRO PARAMETERS ERROR","Parameter ("+str(i+1)+") for macro \""+funcName+"\" defined at line '"+str(lineN)+" has not been defined with a corresponding @cpptype. All parameters need to have this tag so that the wrapper knows what to do");
                   return;
               #get the parameter type and from the pString get the value
               part = tempPString.partition(",")[0];
               self.types.append(pType);
               self.values.append(part[0]);
               tempPString = part[2];

            #since I am using it in some places, reconstruct the pstring
            self.pString = "";
            for i in range(self.number):
                self.pString += self.types[i]+" "+self.values[i]+",";
            #lose the last comma
            self.pString = self.pString.rstrip(',');

        #normal function
        else:
            self.number = 0;
            newString = "";
            while True:
                part = self.pString.partition(",");
                if(part[1] == ""):
                    part = self.pString.partition(")");
                    if(part[1] == ""):
                        self.pString = newString.rstrip(', ');
                        break;
                #else
                #print("Parsing parameters for function \""+funcName+"\" and pString is \""+self.pString+"\"");
                #also partition with " " to get the type
                #take in the types(morepart[0]) and values(morepart[2]) of the parameters
                p = part[0];
                p = p.rstrip(); #make sure we got no trailing spaces
                morepart = p.rpartition(" ");
                pType = morepart[0];
                #replace the struct pointers with class references so it's more C++ish. (Unless the keep pointer tag is on the parameter)
                ancestorCheck = hasAncestor(morepart[0],ancestorsL);
                if(ancestorCheck[1] != -1):#if an ancestor is found in the parameter
                    #if we need to keep the pointer
                    if(checkPFlag(self.flags,self.number,PARAMS.KEEPPTR) == -1):
                        if(ancestorCheck[1] == 1):
                            printError("FUNCTION PARAMETER PARSING","During parsing the parameters of \""+funcName+"\" a class name was found in the original parameters of a C function. This should never happen");
                            return;
                        #print("Replacing \""+ancestorCheck[0]+"*\" with \""+ancestorCheck[2]+"&\"");
                        p = p.replace(ancestorCheck[0]+"*",ancestorCheck[2]+"&");
                        pType = morepart[0].replace(ancestorCheck[0]+"*",ancestorCheck[2]+"&");
                    else:
                        #print("Replacing \""+ancestorCheck[0]+"\" with \""+ancestorCheck[2]+"\"");
                        p = p.replace(ancestorCheck[0],ancestorCheck[2]);
                        pType = morepart[0].replace(ancestorCheck[0],ancestorCheck[2]);
                    newString+=p;
                #check the void* and if they have an @inhtype tag then replace them with class reference
                elif(part[0].find("void*") != -1):
                    inhType =checkPFlag(self.flags,self.number,PARAMS.CLASSOBJECT);
                    if(inhType != -1):
                        p = p.replace("void*",inhType+"&");
                        pType = morepart[0].replace("void*",inhType+"&");
                        newString+= p;
                    else:
                        newString+= p;
                #nothing to be done, take the parameter as it is
                else:
                    newString += p;
                ancestorCheck[:] = [];
                #take in the types and values of the parameters
                self.types.append(pType);
                self.values.append(morepart[2]);
                #an extra check for var args
                if(self.values[self.number].find("...") != -1):
                    self.hasVarArgs = True;
                    self.values[self.number] = "...";
                    self.types[self.number] = "...";
                #increase paramsN and go to the next param
                self.number +=1;
                self.pString = part[2];
                newString+=", "

        #test for params number being the same as the param flags from comments
        if(self.number != len(paramFlags) and self.hasVarArgs == False):
            #FAILURE
            print("At line: '"+str(lineN)+"' during Initialization of function '"+funcName+"' for struct \""+structName+"\" the number of detected parameters '"+str(self.number)+"' did not match the parameters number declared in the comment block '"+str(len(paramFlags))+"'");
            raise CtorError;

    #! A function to clear the parameters lists
    def clear(self):
        self.values[:] = [];
        self.types[:] = [];
        self.flags[:] = [];


#! The C Function class
class CFunction:
    funcName="";
    retType="";
    cFuncSig="";
    structName="";
    className="";
    ctor = False;
    isMacro = False;
    isVirtual = False;
    readyCode = [];
    baseStruct = "";
    #! Initializes a a function object
    #! @param line The line to parse the function from
    #! @param imex The import export macro
    #! @param searchFuncSig The signature of the type of functions the parser is searching
    #! @param ctor True or false if this a constructor or not
    #! @param notctor True if this is a constructor type function for C but should not be one in C++ and false otherwise
    #! @param structName The name of the structure for which this function mainly works
    #! @param className The name of the class whose member this function wiull be
    #! @param ancestors A list of structs that this struct inherits from, starting from the oldest to the newest
    #! @param paramFlags A list of parameter flags passed by the parser. They are special flags that are found inside the comment blocks in the C code. Each one if a tuple of flag type and flag value
    #! @param returnFlags Optional extra information for the return of the function. Each one is a tuple of flag type and flag value.
    #! @param lineN The number of the line where the function was found
    #! @param readyCode This is a list which most of the time would be empty. If it is not then this means that we have to ignore everything and use the given function for wrapper
    #! @param isVirtual A boolean flag denoting if this function should be declared virtual or not
    #! @param operator An operator that can either be NONE or any of the other operator values to determine that this function should also be treated as an operator with the same arguments
    #! @param comesfromdefine A boolean flag indicating that this wrappable function originally comes from a define
    def __init__(self,line,imex,searchFuncSig,ctor,notctor,structName,className,ancestors,baseStruct,baseClass,paramFlags,returnFlags,lineN,readyCode,isVirtual,operator,comesfromdefine):
        self.cFuncSig = searchFuncSig;
        self.structName = structName;
        self.className = className;
        self.ctor = ctor;
        self.operator = operator;
        self.comesfromdefine = comesfromdefine;
        self.returnFlags = returnFlags;
        #get the ancestors and the basename
        self.ancestors = ancestors;
        #get the base struct and base class names
        self.baseStruct = baseStruct;
        self.baseClass = baseClass;

        #a flag that will only have meaning if this was a constructor function that is not interpreted as one
        self.notctor = False;
        self.readyCode = readyCode;
        self.isVirtual = isVirtual
        #if no ready code is given then that's good. We proceed with the wrapping. Else we do nothing. The ready code does everything for us
        if(len(readyCode) == 0):
            #if we export the function then make sure to avoid it in the cpp file
            if(line.startswith(imex)==True):
                line = line.lstrip(imex);
                line = line.lstrip();
            #get the return type and make sure that if its this struct ptr to return this class reference
            partition = line.partition(searchFuncSig);
            self.retType = partition[0];
            self.retType = self.retType.rstrip();
            self.retType = self.retType.lstrip();
            ancestorCheck = hasAncestor(self.retType,self.ancestors);
            if(ancestorCheck[1] != -1):
                if(ancestorCheck[1]==1):
                    printError("FUNCTION INITIALIZATION","Error in Function initialization because at the RETURN TYPE of the function a className was found inside C code. This should never happen");
                    return;
                self.retType = self.retType.replace(ancestorCheck[0],ancestorCheck[2]);
            ancestorCheck[:] = [];
            #also if this comes from a define then self.retType must now contain "#define", so replace it with the actual value from the comment tag
            if(comesfromdefine):
                self.retType = ""
                #if we have no information from the return flag tag then consider it void
                if(len(returnFlags) == 0):
                    self.retType = "void";
                #else just check the return flags for a return type. Note that the retype tag should contain the CPP type of return so no extra checks here
                else:
                    for i in returnFlags:
                        if(i[0] == "rettype"):
                            self.retType = i[1];
                #if we still get here with no rettype then we have a problem
                if(self.retType == ""):
                    printError("FUNCTION INITIALIZATION","During initialization of a function from a #define at line '"+str(lineN)+"' the return type could not be understood");
            #get the function name
            line = partition[2];
            partition = line.partition("(");
            self.funcName = partition[0];
            #if it's a ctor that we will turn to a function then replace init with from
            if(ctor == True and notctor == True):
                self.funcName = self.funcName.replace("Create","From");
                self.ctor = False;
            try:
                loseFirst = True;
                if(self.funcName.find("Create") != -1):
                   loseFirst = False;
                   self.notctor = True;
                #Initialize the parameters object by getting the string after the name of the function
                self.params= FunctionParameters(partition[2],loseFirst,structName,className,paramFlags,lineN,self.funcName,comesfromdefine,self.ancestors);
            except CtorError:
                print("Error!");
                print("Initializing the parameters attribute of the function failed. Function initialization failed");
                raise CtorError;
            #at the moment if the function has a var arg parameter it automatically means that the wrapping will be a macro so
            self.isMacro = self.params.hasVarArgs;





    #! prints this function to the given cpp output file
    #! @param outputS The output source file handler
    #! @param outputH The output header file handler
    def printToCPP(self,outputS,outputH):
        #==If Ready Code has been given==#
        if(len(self.readyCode) > 0):
            outputH.write("\t"+(self.readyCode[0].replace(self.className+"::","")).rstrip('\n')+";\n");
            outputS.write("\t"+self.readyCode[0]);
            for codeline in self.readyCode[1:]:
                outputS.write(codeline);

        #==If the function has var args we are gonna define it as a macro==#
        elif(self.isMacro == True):
            self.printCPP_IsMacro(outputS,outputH);
        #==no vararg parameters, normal function==#
        else:
            self.printCPP_IsFunc(outputS,outputH);

            #finally if the function need to overload some operator, take care of that too
            self.printOperatorOverloading(outputS,outputH);

    #! Prints the  function to the cpp file if it's a macro
    def printCPP_IsMacro(self,outputS,outputH):
            outputH.write("\t#define "+self.className+"_"+self.funcName+"(i_thisptr");
            #write all the parameters of the macro declaration
            for i in self.params.values:
                if(i != "..."):
                    outputH.write(",i_"+i+"_");
                else:
                    outputH.write(",...");
            #now close and go to macro definition, whose first parameter should be the this handle
            outputH.write(")\t"+self.cFuncSig+self.funcName+"(("+self.structName+"*)i_thisptr->i_"+self.baseClass+"CHandle");
            #write all the parameters of the macro definition and make sure to replace ... with __VA_ARGS__
            count = 0;
            for pName in self.params.values:
                ancestorCheck = hasAncestor(self.params.types[count],self.ancestors);
                if(pName == "..."):#is vararg
                    outputH.write(",__VA_ARGS__");
                elif(ancestorCheck[1] != -1) :#if we have an ancestor
                    if(ancestorCheck[1] == 0):#if it's a structure
                        printError("FUNCTION CPP PRINTING","During printing function \""+self.funcName+"\"to the CPP file as a macro a structure was found in the parameters. The error occured for \""+pName+"\" This should not happen");
                    #should be a class ancestor so..
                    outputH.write(",("+ancestorCheck[0]+"*)i_"+pName+"_.i_"+self.baseClass+"CHandle");
                else:# all other params
                    outputH.write(",i_"+pName+"_");
                count+=1;
                ancestorCheck[:] = [];
            #close and finish
            outputH.write(")\n");

    #! Prints the  function to the cpp file if it's a normal function
    def printCPP_IsFunc(self,outputS,outputH):
            ##==HEADER==##
            #print the function signature in the header file depending on if it's a constructor or not
            if(self.ctor == True):
                if(self.params.number > 0):
                    outputH.write("\t"+self.className+"("+self.params.pString+");\n");
                else:
                    outputH.write("\t"+self.className+"();\n");
            else:
                outputH.write("\t");
                #if we got a virtual function
                if(self.isVirtual):
                    outputH.write("virtual ");
                if(self.params.number > 0):
                    outputH.write(self.retType+" "+self.funcName+"("+self.params.pString+");\n");
                else:
                    outputH.write(self.retType+" "+self.funcName+"();\n");
            ##==SOURCE==##
            returnPtr = False;
            #if we got any parameters
            if(self.params.number >0):
                if(self.ctor == True):
                    outputS.write(self.className+"::"+self.className+"("+self.params.pString+"){\n\t");
                else:
                    outputS.write(self.retType+" "+self.className+"::"+self.funcName+"("+self.params.pString+"){\n\t");
                    if(self.retType != "void"):
                        ancestorCheck = hasAncestor(self.retType,self.ancestors);
                        #check if any ancestor is returned
                        if(ancestorCheck[0] != ""):
                            if(ancestorCheck[1] == 0):
                                printError("CPP FUNCTION PRINTING","In printing  function \""+self.funcName+"\" in a cpp file there was an error in return type parsing. It is a struct while it should be a class because of checks in CFunction.__init__()");
                            outputS.write(ancestorCheck[0]+"* cstruct = ")
                            returnPtr = True;
                        else:
                            outputS.write("return ");
                        ancestorCheck[:] = [];
                #constructor call
                if(self.ctor == True):
                    outputS.write("this->i_"+self.baseClass+"CHandle = ("+self.structName+"*) "+self.cFuncSig+self.funcName+"(");
                    for i in range(self.params.number):
                        ancestorCheck = hasAncestor(self.params.types[i],self.ancestors);
                        #write it to the output source file depending on the type. All types are just copied except if it's an ancestor in which case we gotta input the C handle as a parameter
                        if(ancestorCheck[1]!=-1):
                            if(self.params.types[i].find(ancestorCheck[2]+"&") != -1):
                                outputS.write(self.params.values[i]+".i_"+self.baseClass+"CHandle")
                            elif(self.params.types[i].find(ancestorCheck[2]+"*") != -1):
                               outputS.write(self.params.values[i]+"->i_"+self.baseClass+"CHandle")
                            else:
                                printError("CPP FUNCTION PRINTING","During printing  constructor function \""+self.funcName+"\" an ancestor was found in the parameters with an unknown type. Do not know how to handle it");
                        else:
                            outputS.write(self.params.values[i]);
                        #if it's not the last parameter put a comma, if it's the last a parentheses
                        if(i != self.params.number-1):
                            outputS.write(",");
                        else:
                            outputS.write(")");
                        ancestorCheck[:] = [];
                #normal function call
                else:
                    #parameters to the C function call if it is a normal call,starting with the this pointer
                    if(self.notctor != True):
                        outputS.write(self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.baseClass+"CHandle,");
                    else:#parameters if it is a _Create call that was not turned into a constructor. (Basically avoiding the tthisptr since it's absent in _Create functions
                        outputS.write(self.cFuncSig+self.funcName+"(");
                    for i in range(self.params.number):
                        ancestorCheck = hasAncestor(self.params.types[i],self.ancestors);
                        #write it to the output source file depending on the type. All types are just copied except if it's an ancestor in which case we gotta input the C handle as a parameter
                        if(ancestorCheck[1]!=-1):
                            if(self.params.types[i].find(ancestorCheck[2]+"&") != -1):
                               outputS.write("("+ancestorCheck[0]+"*)"+self.params.values[i]+".i_"+self.baseClass+"CHandle");
                            elif(self.params.types[i].find(ancestorCheck[2]+"*") != -1):
                               outputS.write("("+ancestorCheck[0]+"*)"+self.params.values[i]+"->i_"+self.baseClass+"CHandle");
                            else:
                                printError("CPP FUNCTION PRINTING","During printing function \""+self.funcName+"\" an ancestor was found in the parameters with an unknown type. Specifically the error occured for parameter type \""+self.params.types[i]+"\". Do not know how to handle this parameter type");

                        else:
                            outputS.write(self.params.values[i]);
                        #if it's not the last parameter put a comma, if it's the last a parentheses
                        if(i != self.params.number-1):
                            outputS.write(",");
                        else:
                            outputS.write(")");
                        ancestorCheck[:] = [];
            else:#0 parameters
                if(self.ctor == True):
                    outputS.write(self.className+"::"+self.className+"(){\n\t");
                else:
                    outputS.write(self.retType+" "+self.className+"::"+self.funcName+"(){\n\t");
                    if(self.retType != "void"):
                        ancestorCheck = hasAncestor(self.retType,self.ancestors);
                        #check if any ancestor is returned
                        if(ancestorCheck[0] != ""):
                            if(ancestorCheck[1] == 0):
                                printError("CPP FUNCTION PRINTING","In printing  function \""+self.funcName+"\" in a cpp file there was an error in return type parsing. It is a struct while it should be a class because of checks in CFunction.__init__()");
                            outputS.write(ancestorCheck[0]+"* cstruct = ")
                            returnPtr = True;
                        else:
                            outputS.write("return ");
                        ancestorCheck[:] = [];
                outputS.write(self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.baseClass+"CHandle)");
            #close the call
            if(returnPtr == True):
                outputS.write(";\n\t"+self.className+"* retobj = new "+self.className+"(cstruct);\n\t//make sure there is no memory leak\n\tfree(cstruct);\n\treturn retobj;\n}\n\n");
            else:
                outputS.write(";\n}\n\n");

    #in the very end and if there are any operator to overload with this function let's do so
    #! Creates and prints operator overloading function from this function. If the value is OPERATORS.NONE then nothing happens
    def printOperatorOverloading(self,outputS,outputH):
        operators = {
        OPERATORS.NONE : lambda x,y: None,
        OPERATORS.ASSIGNMENT : self.assignmentOperator,
        OPERATORS.ADDITION :   self.additionOperator,
        OPERATORS.COMPARISONEQ : self.comparisonEqOperator,
        }
        func = operators[self.operator];
        func(outputS,outputH);


    #ASSIGNMENT OPERATOR
    def assignmentOperator(self,outputS,outputH):
        if(self.params.number != 1):
            printError("ASSIGNMENT OPERATOR OVERLOADING","In the assignment operator overloading for function \""+self.funcName+"\" a number of parameters different than 1 was given");
        #header file
        outputH.write("\t"+self.className+"& operator=("+self.params.types[0]+" "+self.params.values[0]+");\n");
        #source file
        outputS.write(self.className+"& "+self.className+"::operator=("+self.params.types[0]+" "+self.params.values[0]+")\n{\n");
        outputS.write("\tif(this->i_"+self.className+"CHandle != 0)\n");
        outputS.write("\t\t"+self.cFuncSig+"Destroy(("+self.structName+"*)this->i_"+self.className+"CHandle);\n");
        outputS.write("\tthis->i_"+self.className+"CHandle = ("+self.structName+"*) "+self.cFuncSig+self.funcName+"("+self.params.values[0]+");\n\treturn *this;\n}\n");
    #ADDITION OPERATOR
    def additionOperator(self,outputS,outputH):
        if(self.params.number != 1):
            printError("ADDITION OPERATOR OVERLOADING","In the addition operator overloading for function \""+self.funcName+"\" a number of parameters different than 1 was given");
        #header file (declaring both + and += operators)
        outputH.write("\t"+self.className+"& operator+=("+self.params.types[0]+" "+self.params.values[0]+");\n");
        outputH.write("\tconst "+self.className+"& operator+("+self.params.types[0]+" "+self.params.values[0]+");\n");
        #source file (defining both + and += operator)
        outputS.write(self.className+"& "+self.className+"::operator+=("+self.params.types[0]+" "+self.params.values[0]+")\n{\n");
        if(self.params.types[0].find(self.className+"&")!=-1):
             outputS.write("\t"+self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.className+"CHandle,"+self.params.values[0]+".i_"+self.className+"CHandle);\n\treturn *this;\n}\n");
        else:
            outputS.write("\t"+self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.className+"CHandle,"+self.params.values[0]+");\n\treturn *this;\n}\n");

        outputS.write("const "+self.className+"& "+self.className+"::operator+("+self.params.types[0]+" "+self.params.values[0]+")\n{\n");
        if(self.params.types[0].find(self.className+"&")!=-1):
            outputS.write("\t"+self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.className+"CHandle,"+self.params.values[0]+".i_"+self.className+"CHandle);\n\treturn *this;\n}\n");
        else:
            outputS.write("\t"+self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.className+"CHandle,"+self.params.values[0]+");\n\treturn *this;\n}\n");
    #COMPARISON (EQUALS / NEQUALS) operator
    def comparisonEqOperator(self,outputS,outputH):
        if(self.params.number != 1):
            printError("COMPARISON EQUALS/NEQUALS OPERATOR OVERLOADING","In the equal/nequal comparison operator overloading for function \""+self.funcName+"\" a number of parameters different than 1 was given");
        #make sure that the parameter is constant for the comparison
        if(self.params.types[0].find("const") == -1):
           self.params.types[0] = "const " + self.params.types[0];
        #header file (declaring both == and != operators)
        outputH.write("\tbool operator==("+self.params.types[0]+" "+self.params.values[0]+") const;\n");
        outputH.write("\tbool operator!=("+self.params.types[0]+" "+self.params.values[0]+") const;\n");
        #source file (defining both == and != operators)
        outputS.write("bool "+self.className+"::operator==("+self.params.types[0]+" "+self.params.values[0]+") const\n{\n");
        if(self.params.types[0].find(self.className+"&") != -1):
            outputS.write("\treturn "+self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.className+"CHandle,("+self.structName+"*)"+self.params.values[0]+".i_"+self.className+"CHandle);\n}\n");
        else:
            outputS.write("\treturn "+self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.className+"CHandle,"+self.params.values[0]+");\n}\n");

        outputS.write("bool "+self.className+"::operator!=("+self.params.types[0]+" "+self.params.values[0]+") const\n{\n");
        if(self.params.types[0].find(self.className+"&")!=-1):
            outputS.write("\treturn !("+self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.className+"CHandle,("+self.structName+"*)"+self.params.values[0]+".i_"+self.className+"CHandle));\n}\n");
        else:
            outputS.write("\treturn !("+self.cFuncSig+self.funcName+"(("+self.structName+"*)this->i_"+self.className+"CHandle,"+self.params.values[0]+"));\n}\n");


    #! Clears all the function related data
    def clear(self):
        if(len(self.readyCode) == 0):
            self.params.clear();
            self.returnFlags[:] = [];
        else:
           self.readyCode[:] = [];
        #we are not deleting the ancestors list since it is cleared only at the end one the run for one Struct by the input object
