#---------------------------------------------------------------------------------------#
# @author Lefteris                                                                      #
# @date 07/05/2012                                                                      #
#                                                                                       #
# This is a python script whose purpose is to parse a C file where there is a C         #
# structure defined in a specific way (using the Formatting                             #
# I am usually using in my projects) and attempts to spit out a header and source       #
# file that will contain a C++ wrapper of that very structure                           #
# To configure the input to this script open the input file which should be in the      #
# same directory as this script and by reading the comments of each input configure     #
# it properly.                                                                          #
#---------------------------------------------------------------------------------------#

#import all the functions and the two classes from the cpp wrapper
from rf_parser import Parser
#import the wrap error exception
from rf_input import WrapError
#to be able to exit prematurely
import sys

#! This function parses the whole file from the input
def parseFullFile(inpfilename,verbose):
    #read input and initialize the wrapper object
    p = Parser(inpfilename,verbose);

    try:
        #parse all the functions
        while True:
            if(p.parseFunction("") == False):
                break;
    except WrapError:
        print("ERROR ENCOUNTERED - WRAPPER TERMINATED ABRUPTLY");
        p.clear();
        return;

    #should have reached the end here so ...
    p.printEnd();
    print("STOPPED Reading at line "+str(p.inLineN));
    p.clear();

#! Just a test function to try and parse only one function
def parseTest(inpfilename,funcname):
    p = Parser(inpfilename,True);
    try:
        p.parseFunction(funcname);
    except WrapError:
        print("ERROR ENCOUNTERED - WRAPPER TERMINATED ABRUPTLY");
        p.clear();
        return;
    p.printEnd();
    print("STOPPED Reading at line "+str(p.inLineN));
    p.clear();


#parseFullFile("RF_String.wrapper.input",True);
parseFullFile("RF_StringX.wrapper.input",False);


#parseTest("RF_StringX.wrapper.input","rfStringX_BeforeMoveP_IN");





