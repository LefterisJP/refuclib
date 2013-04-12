#-------------------------------------------------------------------------------
# @author Lefteris
# @date 22/05/2012
#
# This is a script that should run once right after doxygen is run for the first
# time. Then unfortunately doxygen needs to be run again once more. At the moment
# I can not think of a smarter way to accomplish this. It's in the todo list.
#-------------------------------------------------------------------------------

from gen_rf_errorslist_html import generate_Errors_List
from Inheritance import checkInheritedFunctions
from TocCreation import createToc
from shutil import copyfile
from html import processHTML,processStartPage
from ProjectPaths import *
from subpage import createSpecialSubpage,undoSpecialSubpage
from args import Args


import sys
import subprocess
import shutil
import os

#The pages for which to create the Table Of Contents
doxygenPagesList = ["RF_ErrorLoggingPGE.html",
                    "RF_LMSPGE.html",
                    "RF_GeneralInformationPGE.html",
                    "RF_ThreadingPGE.html",
                    "RF_TimePGE.html",
                    "RF_StringsPGE.html",
                    "RF_PreprocessorPGE.html",
                    "RF_XMLPGE.html",
                    "RF_DataStructuresPGE.html",
                    "RF_SystemPGE.html",
                    "RF_IOPGE.html",
                    "RF_BuilderPGE.html",
                    "RF_ContributePGE.html",
                    "RF_RoadmapPGE.html",
                    "RF_TestingFrameworkPGE.html"
                   ]

inheritedList = ["RF_StringX"]



print("ATTEMPTING TO REMOVE THE PREVIOUS html DIRECTORY")
try:
    shutil.rmtree(htmldocspath);
except:
    pass
#read in the command-line arguments
args = Args();
print("BUILDING REFU DOCUMENTATION...");
#create the special subpages relation
createSpecialSubpage()
#sys.exit(0)

#run doxygen once first
print("Running doxygen for the first time...")
subprocess.call(doxygenpath+" "+args.configFile);
print("First doxygen run concluded...");
#generate the errors list
generate_Errors_List();
#run doxygen again
print("Running doxygen for the second time...")
subprocess.call(doxygenpath+" "+args.configFile);
print("Second doxygen run concluded")
checkInheritedFunctions(inheritedList,htmldocspath)
#The ToC Creation should be the last thing on the list since it adds the index to the doxygen pages list
createToc(doxygenPagesList,htmldocspath)
#Process the startpage and add a few things that doxygen can't add on its own
processStartPage(htmldocspath)
#Process all html files making necessary changes
processHTML(htmldocspath)
#undo the special subpage changes to the dox files
undoSpecialSubpage()
#finally copy the favicon.ico and other used images to the html dir
shutil.copy(rootpath+"\\Pics\\favicon.ico",rootpath+"\\docs\\html\\favicon.ico");
for root, dirs, files in os.walk(os.path.join(rootpath,"docs","images")):
        for f in files:
            fullname = os.path.join(root,f);
            shutil.copy(fullname,os.path.join(rootpath,"docs","html"))

#at the end delete the extra configuration file if needed
if(args.private):
    os.remove(doxyconfigfile+"_temp");
print("FINISHED BUILDING REFU DOCUMENTATION");




