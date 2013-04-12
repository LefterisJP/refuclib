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

import linecache

from rf_input import printError,printWarning
from rf_cfunc import OPERATORS, PARAMS


#! Processes a comment tag and takes its value. Returns the comment line without the tag
#! @param line The comment line in which the tag was found
#! @param tag the tag itself with the '@' mark
#! @return Returns a list with the first being the line string without the tag and the rest being the values of the tag. If there is an error it returns a list with the first object being an empty string
def extractValueFromTag(line,tag):
    newLine = "";
    returnList = [];
    part = line.partition(tag+"{");
    if(part[1] == ""):
        return [""];
    newLine += part[0];
    #get what's between the two { }
    tempS = part[2];
    part = tempS.partition("}");
    if(part[1] == ""):
        return [""];
    #create the line without the tag
    newLine += part[2];
    returnList.append(newLine);
    #now get all the values
    tempS = part[0];
    part = tempS.partition(",");
    #as long as we find a comma
    while(part[1] != ""):
        part = tempS.partition(",");
        returnList.append(part[0]);
        tempS = part[2];
    #also add the last tag
    returnList.append(tempS);
    #finally return
    return returnList;



#Parsing of comment blocks
class CommentParser:
    paramFlags = [];
    returnFlags = [];
    readyCode = [];
    virtualFunc = False;
    operator = OPERATORS.NONE;
    #A flag denoting that we are now parsing a C Init(Constructor) function that should be interpreted as a simple function. The @cppnotctor tag
    notctor = False;

    #! Initializes a comment block object
    def __init__(self,inputFile,structName,className,ctor):
        #the list of lines that will make up the comment
        retLines = [];
        #reset the ready code list
        self.readyCode[:] = [];
        #initialize some flags
        self.notctor = False;
        self.ctor = ctor;
        #get the required data
        self.inputFileName = inputFile;
        self.structName = structName;
        self.className = className;

    #! A function to process comment blocks of all kind of functions. Takes in useful data about the function and also returns the comments that will go to the cpp function/constructor
    #! @param commentLineN The number of the line where the comment block starts
    #! @param commentLength The length of the comment block in lines
    #! @return Returns a list of comments line string for the comment block belonging to a wrappable function and an empty list if it is not a wrappable function
    def process(self,commentLineN,commentLength):
        paramN = 1;
        inCPPCode = False;
        #first check the special flags set for the cpp wrapper which should be on the first line
        line = linecache.getline(self.inputFileName,commentLineN);
        #the lines list to return
        retLines = [];
        #the flags of this parameter
        thisPFlags = [];
        #start processing all comment lines
        for i in range(commentLength):
            line = linecache.getline(self.inputFileName,commentLineN+i);
            #if we are in cpp code save it inside the ready code list
            if(inCPPCode == True):
                if(line.find("@endcpp") == -1):
                    line = line.replace("//!","");
                    line = line.rstrip('**');
                    self.readyCode.append(line);
                else:
                   inCPPCode = False;
                continue;

            #if this line is a parameter line
            elif(line.find("@param") != -1):
                #skip the first @param comment line since it's always THIS pointer EXCEPT for the constructor _Create functions case
                if(paramN == 1):
                    #if this is not a constructor, skip the first
                    if(self.ctor == False and self.notctor == False):
                        paramN+=1;
                        continue;
                #if we got the @inhtype tag
                if(line.find("@inhtype") != -1):
                    res = extractValueFromTag(line,"@inhtype");
                    if(res[0] == ""):
                        printWarning("CPP TAG USAGE","In line '"+str(commentLineN+i)+"' The @inhtype tag was used in an illegal way");
                    #make sure that the new comment line does not contain it
                    line = res[0];
                    #note the first of the list it in the params flag. Remember that this will be the class name of the oldest ancesto that should be used for the void*
                    thisPFlags.append([PARAMS.CLASSOBJECT,res[1]]);
                    res[:] = [];

                #if we got the keepptr tag
                elif(line.find("@keepptr") != -1):
                    #self.paramFlags.append([PARAMS.KEEPPTR,""]);
                    thisPFlags.append([PARAMS.KEEPPTR,""]);
                    line = line.replace("@keepptr","");
                #if we got a variable argument
                elif(line.find("...")!=-1):
                    #loop all the comments beforehand and obfuscate their names a bit since this is gonna be a macro.
                    for k in range(len(retLines)):
                        if(retLines[k].find("@param") != -1):
                            newString = "\t//! @param ";
                            part = retLines[k].partition("@param ");
                            tempS = part[2];
                            tempS = tempS.lstrip();
                            part = tempS.partition(" ");
                            newString += "i_"+part[0]+"_ ";
                            newString += part[2];
                            retLines[k] = newString;
                    #Also add 1 more comment for the this parameter
                    retLines.append("\t//! @param i_thisptr Pass a pointer to the object for which you need the funtion to work for here\n");
                #if we got the @cpptype tag which specifies what type of parameter this will be in the cpp code. (Mostly used in macros)
                elif(line.find("@cpptype") != -1):
                    res = extractValueFromTag(line,"@cpptype");
                    if(res[0] == ""):
                        printWarning("CPP TAG USAGE","In line '"+str(commentLineN+i)+"' The @cpptype tag was used in an illegal way");
                        continue;
                    line = res[0];
                    #since for now I know that @cpptype has only 1 value
                    thisPFlags.append([PARAMS.GIVENTYPE,res[1]]);
                    res[:] = [];


                else:
                    thisPFlags.append([PARAMS.NORMAL,""]);
                #also increase paramn
                paramN+=1;
                #finally AFTER finishing parsing This parameter's flags add them to all the param flags
                self.paramFlags.append(thisPFlags);
            #if it's just any other type of comment line [not an @param line)
            else:
                #if this should be ignored by the wrapper script quit the function
                if(line.find("@cppignore") != -1):
                    retLines[:] = [];
                    return retLines;
                #if this function should also produce an assignment operator overload
                if(line.find("@opassign") != -1):
                    if(self.operator != OPERATORS.NONE):
                        printWarning("COMMENT PARSING","During parsing a comment block in line '"+str(commentLineN+i)+"' an operator tag @opassign was found while there has already been set a previous operator overloading somewhere in the very same block");
                    self.operator = OPERATORS.ASSIGNMENT;
                    continue;
                #if this function should also produce an addition operator overload
                if(line.find("@opadd") != -1):
                    if(self.operator != OPERATORS.NONE):
                        printWarning("COMMENT PARSING","During parsing a comment block in line '"+str(commentLineN+i)+"' an operator tag @opadd was found while there has already been set a previous operator overloading somewhere in the very same block");
                    self.operator = OPERATORS.ADDITION;
                    continue;
                #if this function/macro should also produce an equal/nequal comparison overload
                if(line.find("@opcmpeq") != -1):
                    if(self.operator != OPERATORS.NONE):
                        printWarning("COMMENT PARSING","During parsing a comment block in line '"+str(commentLineN+i)+"' an operator tag @opassign was found while there has already been set a previous operator overloading somewhere in the very same block");
                    self.operator = OPERATORS.COMPARISONEQ;
                    continue;
                if(line.find("@cppnotctor") != -1):
                    self.ctor = False;
                    self.notctor = True;
                    continue;
                #the doxygen @memberof should stay only in C
                if(line.find("@memberof")!=-1):
                   continue;
                #for the @return tag
                if(line.find("@return") != -1):
                    #it it's a ctor just ignore the line
                    if(self.ctor == True):
                        continue;
                    #else check if we have the @cpprettype tag
                    if(line.find("@cpprettype") != -1):
                        res = extractValueFromTag(line,"@cpprettype");
                        if(res[0] == ""):
                            printWarning("CPP TAG USAGE","In a comment block in line '"+str(commentLineN+i)+"' The @cpprettype tag was used in an illegal way");
                            continue;
                        line = res[0];
                        #since for now I know that @cpprettype has only 1 value
                        self.returnFlags.append(["rettype",res[1]]);
                        res[:] = [];
                #if we have cpp code given then things change
                if(line.find("@cppcode") != -1):
                    inCPPCode = True;
                    continue;
                #if we have a cpp snippet
                if(line.find("@snippet") != -1):
                    #check the next line to see there is a CPP equivalent
                    nextLine = linecache.getline(self.inputFileName,commentLineN+i+1)
                    if(nextLine.find("@cppsnippet") != -1):
                        continue;#go to the next line without writing this line
                    #else just go and write it normally
                if(line.find("@cppsnippet") != -1):
                    #check the previous line to get the file
                    prLine = linecache.getline(self.inputFileName,commentLineN+i-1);
                    part = prLine.partition("@snippet ");
                    if(part[1] == ""):
                        printWarning("FUNCTION COMMENTS","During parsing a function's comments block at line "+str(commentLineN+i)+" an incorrectly positioned @cppsnippet tag was found");
                        continue;
                    #if we found the @snippet tag in the line above get the file name from there
                    fileName = part[2].rstrip();
                    fileName = fileName.partition(" ")[0];
                    #so now write this line
                    line = line.replace("@cppsnippet","@snippet "+fileName+" ");
                #if you find the @isinherited tag, since I always put it in a line of its own, we can safely skip the line
                if(line.find("@isinherited")!=-1):
                    continue;
                #if you find the @notinherited tag, it means we have inheritance. Also since I always put it in a line of its own we can safely skip the line
                if(line.find("@notinherited") != -1):
                    self.virtualFunc = True;
                    continue;


            #replace all occurences of the struct name in the comment with class name
            line = line.replace(self.structName,self.className);
            #finally append the line to the list
            retLines.append("\t"+line);

        #if we reach the end and we are still parsing CPP code then that's an error
        if(inCPPCode == True):
            retLines[:] = [];
            printError("FUNCTION COMMENTS","During parsing a function's comments starting at line "+str(commentLineN)+" an unterminated @cppcode tag was found");
        #TEMP TEST START
        #pN = len(self.paramFlags);
        #print("The comment block found '"+str(pN)+"' parameters");
        #for i in range(pN):
        #    pList = self.paramFlags[i];
        #    lll = len(pList);
        #    print("Going in the list of the '"+str(i)+"' parameter which has '"+str(lll)+"' length");
        #    for j in range(lll):
        #        ppp = pList[j];
        #        print("Flag Type: \""+str(ppp[0])+"\" Flag Value \""+str(ppp[1])+"\"");
        #    print("Going out the list of the '"+str(i)+"' parameter");
        #TEMP TEST END
        #finally return the comment lines
        return retLines;


    def clear(self):
        #clear the parameters flags
        for pList in self.paramFlags:
            lll = len(pList);
            for param in pList:
                param[:] = [];#clear this type and value tuple
            pList[:] = []; #clear this parameter's flag list
        #clear the whole parameters flag list
        self.paramFlags[:] = [];
        #clear the return flags
        for flag in self.returnFlags:
            flag[:] = [];
        #DONT clear the ready code. It's cleaned by the  CFunction.clear()
        #self.readyCode[:] = [];
