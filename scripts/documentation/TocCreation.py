import os




# Creates a table of contents by reading the lines of the page files
def createToc(pages,htmldocspath):
    # Creata ToC for all the pages
    pages.append("index.html");
    level = 0;
    for page in pages:
        tableofContents = [];
        used = [False,False,False];
        tableofContents.append("<div class=\"rf_toc\"><h1 class=\"rf_toc\">TABLE OF CONTENTS</h1><ul class=\"rf_toc1\">");#first line of the table of contents
        inF = open(htmldocspath+"\\"+page,"r",encoding="utf8");
        print("Creating table of contents for "+page);
        for line in inF:
            #if we have a line with an @section
            if("<h1><a class=\"anchor\" id=\"" in line):
                if(level != 0):
                    if(level > 1):
                        for i, val in enumerate(used[1::]):
                            if(val == True):
                                used[i+1] = False;
                                tableofContents.append("</ul>");
                level = 1;
                used[0] = True;

                part = line.partition("<h1><a class=\"anchor\" id=\"")[2];
                morepart = part.partition("\"");
                idStr = morepart[0];
                line = inF.readline();
                title = line.partition("</h1>")[0];
                tableofContents.append("<li class=\"rf_toc1\">"+"<a href=\"#"+idStr+"\">"+title+"</a></li>");
                continue;#next line
            #if we have a line with an @subsection
            elif("<h2><a class=\"anchor\" id=\"" in line):
                if(level > 2):
                    tableofContents.append("</ul>");
                    for i, val in enumerate(used[2::]):
                        if(val == True):
                            used[i+2] = False;
                            tableofContents.append("</ul>");
                    tableofContents.append("<ul class=\"rf_toc2\">");
                elif(level != 2):
                    tableofContents.append("<ul class=\"rf_toc2\">");
                level=2;
                used[1] = True;

                part = line.partition("<h2><a class=\"anchor\" id=\"")[2];
                morepart = part.partition("\"");
                idStr = morepart[0];
                line = inF.readline();
                title = line.partition("</h2>")[0];
                tableofContents.append("<li class=\"rf_toc2\">"+"<a href=\"#"+idStr+"\">"+title+"</a></li>");
                continue;#next line
            #if we have a line with an @subsubsection
            elif("<h3><a class=\"anchor\" id=\"" in line):
                if(level != 3):
                    tableofContents.append("<ul class=\"rf_toc3\">");
                level=3;
                used[2] = True;
                part = line.partition("<h3><a class=\"anchor\" id=\"")[2];
                morepart = part.partition("\"");
                idStr = morepart[0];
                line = inF.readline();
                title = line.partition("</h3>")[0];
                tableofContents.append("<li class=\"rf_toc3\">"+"<a href=\"#"+idStr+"\">"+title+"</a></li>");
                continue;#next line
        #at the end  close any open lists and the div
        for val in used:
            if(val == True):
                tableofContents.append("</ul>");
        tableofContents.append("</div>");
        #now read the file again, this time creating the copy with the table of contents
        inF.close();
        inF = open(htmldocspath+"\\"+page,"r",encoding="utf8");
        outF = open(htmldocspath+"\\temp_"+page,"w",encoding="utf8");
        atStart = False;
        #read every line
        for line in inF:
            if(atStart == False):
                #check if we got to where the toc should go
                if("<div class=\"contents\">" in line):
                    #got where we needed to go
                    atStart = True;
                    #toc should go here
                    outF.write("<div class=\"contents\">\n")
                    for s in tableofContents:
                        outF.write(s+"\n");
                else:#just copy each line
                    outF.write(line);
            else:
                #just copy each line until the end of the file
                outF.write(line);
        #close the files and delete the temp and free the tableofcontents string
        tableofContents[:] = [];
        inF.close();
        outF.close();
        os.remove(htmldocspath+"\\"+page);
        os.rename(htmldocspath+"\\temp_"+page,htmldocspath+"\\"+page);
        print("Table of Contents Creation for "+page+" ended");
    print("ALL TOC CREATION FINISHED");
