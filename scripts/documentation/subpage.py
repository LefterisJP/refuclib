from ProjectPaths import *

import os
import shutil
from utils import safeRemove

doxinput = ["RF_DocumentationPGE.dox", "RF_ModulesPGE.dox", "RF_DevelopingPGE.dox"]


def createSpecialSubpage():
    for dox in doxinput:
        fpath = os.path.join(rootpath,"docs","dox_files",dox)
        safeRemove(fpath+"_old")
        os.rename(fpath,fpath+"_old")
        inF = open(fpath+"_old","r",encoding="utf8");
        outF = open(fpath,"w",encoding="utf8");
        for line in inF:#for every line
            if("@rfsubpage " in line):#if the line has the special subpage command
                outF.write(line.replace("@rfsubpage","<li>@subpage").rstrip("\n"))
                found = False;
                #find and open the relevant file
                line = line.partition("@rfsubpage")[2];
                line = line.lstrip(" \t")
                line = line.partition(" ")[0];
                #try and see if we already edited the referenced file
                try:
                    ref= open(os.path.join(rootpath,"docs","dox_files",line+".dox_old"),"r",encoding="utf8")
                except:
                    ref = open(os.path.join(rootpath,"docs","dox_files",line+".dox"),"r",encoding="utf8")
                sections = [];
                sectionLinks = [];

                for refLine in ref:
                    cmd = ""
                    if("@section " in refLine):
                        cmd = "@section"
                    if("@rfsubpage " in refLine):
                        cmd = "@rfsubpage"

                    if(cmd != ""):
                        if(found==False):
                            outF.write("<ol class=\"rfsubpage\">\n")
                            found = True;
                        refLine = refLine.partition(cmd)[2];
                        refLine = refLine.lstrip(" ")
                        sectionLinks.append(refLine.partition(" ")[0])
                        sections.append(refLine.partition(" ")[2].rstrip("\n\"").lstrip("\""))
                ref.close();
                for section,link in zip(sections,sectionLinks):
                    outF.write("<li>@ref {0} \"{1}\"</li>\n".format(link,section))
                if(found):
                    outF.write("</ol>");
                outF.write("</li>\n")
            elif("@rfsubpageliststart" in line):
                outF.write("<ol class=\"rfsubpage\">\n")
            elif("@rfsubpagelistend" in line):
                outF.write("</ol>\n")
            else:#just copy the line normally
                outF.write(line);
        outF.close();
        inF.close();

def undoSpecialSubpage():
    for dox in doxinput:
        fpath = os.path.join(rootpath,"docs","dox_files",dox)
        #just for debugging purposes print it in the stdout
        os.remove(fpath)
        os.rename(fpath+"_old",fpath)






