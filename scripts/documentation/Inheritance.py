#-------------------------------------------------------------------------------
# @author Lefteris
# @date 24/05/2012
#
# This is a script that runs for every html file that contains documentation
# of an inherited structure. It checks which of the parent's structure functions
# should not be inherited by this structure and makes sure they are not in the
# documentation
#-------------------------------------------------------------------------------

import os

# The function that accomplishes the check
def checkInheritedFunctions(inheritedList,htmldocspath):
    print("Check for inherited functions starts...")
    #for every inherited struct
    for struct in inheritedList:
        fileName ="struct"+struct;
        fileName = fileName.replace("_","__");
        inF = open(htmldocspath+"\\"+fileName+".html","r",encoding="utf8");
        outF = open(htmldocspath+"\\"+fileName+"_temp.html","w",encoding="utf8");
        inInherited = False;
        afterInherited = False;
        for line in inF:
            #copy every line until you get to the inherited functions
            if(inInherited == False):
                if("Functions inherited from" in line):
                    inInherited = True;
                    outF.write(line);#write this line too
                    continue;#start parsing from next line
                else:
                    outF.write(line);
            #if we are in the inherited functions
            elif(afterInherited==False):
            #now we are in the inherited functions. We need to check each one of them. So parse until we get to the end of the declarations
                if("</table><table class=\"memberdecls\">" in line):#if we are at the end note it
                    afterInherited = True;
                    outF.write(line);
                    continue;
                #this is an inherited function declaration line so let's take it
                #if it's not a memitem line just copy it
                if("<tr class=\"memitem:" not in line):
                    outF.write(line);
                    continue;

                #take the memitem and the parent
                s =  line.partition("<tr class=\"memitem:")[2];
                s = s.partition("\"")[0];
                parts = s.partition(" ");
                memitem = parts[0];
                parent = parts[2].replace("inherit pub_methods_","");
                #now open the parent file
                print("OPENING: "+parent+"html");
                parentF = open(htmldocspath+"\\"+parent+".html","r",encoding="utf8");
                functionFound = False;keepFunction = True;divEndings = 0;
                for pLine in parentF:
                    if(functionFound==False):
                        #check every line of the parent until you get to the function
                        if("<a class=\"anchor\" id=\""+memitem+"\">" in pLine):
                            functionFound = True;
                    else:#if we are at the function read every line checking to see if we should copy the function or not
                        #count the number of div endings
                        if("</div>" in pLine):
                            divEndings = divEndings+1;
                        if("<em class=\"rf_inability1\">" in pLine):
                            keepFunction = False;
                        #also if we get to the end of the function declaration quit parsing the parent file
                        if(divEndings >= 3):
                            break;
                #close parent file
                parentF.close();

                #if we want to keep the function then write it to the output
                if(keepFunction == True):
                    outF.write(line);
                    line =inF.readline();
                    outF.write(line);
                    continue;
                else:#else skip the line and the next one
                    line = inF.readline();

            else:#we are after the inherited functions, so copy each line
                outF.write(line);

    #close the files and delete the temp
    inF.close();
    outF.close();
    os.remove(htmldocspath+"\\"+fileName+".html");
    os.rename(htmldocspath+"\\"+fileName+"_temp.html",htmldocspath+"\\"+fileName+".html");
    print("Check for inherited functions ended");
