import os

sidebar = "</div>\n\
    <div class=\"rightSideBar\">\n\
    <!-- Right Sidebar Google Ad Right Start-->\n\
    <script type=\"text/javascript\"><!--\n\
    google_ad_client = \"ca-pub-9930161372660082\";\n\
    /* Refudoc_right */\n\
    google_ad_slot = \"8327133651\";\n\
    google_ad_width = 160;\n\
    google_ad_height = 600;\n\
    //-->\n\
    </script>\n\
    <script type=\"text/javascript\"\n\
    src=\"http://pagead2.googlesyndication.com/pagead/show_ads.js\">\n\
    </script>\n\
    <!-- Right Sidebar Google Ad Right End--></div>\n\
    </div><!-- contents -->"


#This function shall be doing some extra processing on the start page. Its original intention
#is rendered null, due to discovering the @htmlonly doxygen tag but there are still
#edits I would like to do to the start page specifically so I am keeping the function here
#for later
def processStartPage(htmldocspath):
    """  DO NOTHING FOR NOW
    print("PROCESSING STARTPAGE")
    inF = open(os.path.join(htmldocspath,"index.html"),"r",encoding="utf8")
    outF = open(os.path.join(htmldocspath,"temp_index.html"),"w",encoding="utf8")
    for line in inF:
        if("$GithubForkButton" in line):
            line = line.replace("$GithubForkButton","")
        if("$GithubWatchButton" in line):
            line = line.replace("$GithubWatchButton","")
        outF.write(line)
    #in the end replace the old file with the temp file and delete the old one
    inF.close()
    outF.close()
    os.remove(os.path.join(htmldocspath,"index.html"))
    os.rename(os.path.join(htmldocspath,"temp_index.html"),os.path.join(htmldocspath,"index.html"))
    print("FINISHED PROCESSING STARTPAGE")
"""

def processHTML(htmldocspath):
    print("INDIVIDUAL PROCESSING OF ALL .html FILES...")
    for item in os.listdir(htmldocspath):
        #for every html FILE of the directory except the documentation Page
        if os.path.isfile(os.path.join(htmldocspath, item)) and item.endswith(".html") and item != "RF_DocumentationPGE.html":
            print("Processing {0}".format(item))
            inF = open(os.path.join(htmldocspath,item),"r",encoding="utf8")
            outF = open(os.path.join(htmldocspath,"temp_"+item),"w",encoding="utf8")
            for line in inF:
                #if this is the line in the file where we want to inject the sidebar
                if(line.find("</div></div><!-- contents -->")!=-1):
                    outF.write(sidebar)
                #else if this is a return type that needs editing
                elif("<td class=\"memname\">" in line):
                    part = line.partition("<td class=\"memname\">");#go after the memname
                    newline = part[0]+part[1]
                    test = part[2]
                    if(test.startswith("<a")):
                        part = test.partition(">")
                        newline = newline+part[0]+part[1]
                        newline = newline+"<em class=\"returnType\">"
                        test = part[2]
                        part = test.partition("</a>");
                        newline = newline+part[0]+"</em>"+part[1]+part[2];
                    else:
                        newline = newline+"<em class=\"returnType\">"
                        part = test.partition(" ");
                        newline = newline+part[0]+part[1]+"</em>"+part[2];
                    outF.write(newline)
                #else just copy the line
                else:
                    outF.write(line)
            #in the end replace the old file with the temp file and delete the old one
            inF.close()
            outF.close()
            os.remove(os.path.join(htmldocspath,item))
            os.rename(os.path.join(htmldocspath,"temp_"+item) , os.path.join(htmldocspath,item))
    #now the documentation page is a special case we have to handle
    print("Removing the double sidebars from the main documentation page")
    inF = open(os.path.join(htmldocspath,"RF_DocumentationPGE.html"),"r",encoding="utf8")
    outF = open(os.path.join(htmldocspath,"temp_RF_DocumentationPGE.html"),"w",encoding="utf8")
    countN = 0;
    for line in inF:
        if(line.find("</div></div><!-- contents -->")!=-1):
            countN=countN+1;
    inF.seek(0,os.SEEK_SET);
    i=0;
    for line in inF:
        if(line.find("</div></div><!-- contents -->")!=-1):
            i=i+1;
        if(i==countN):
            outF.write(sidebar)
            i=i+1;#so that we don't write it again
        else:
            outF.write(line)

    #in the end replace the old file with the temp file and delete the old one
    inF.close()
    outF.close()
    os.remove(os.path.join(htmldocspath,"RF_DocumentationPGE.html"))
    os.rename(os.path.join(htmldocspath,"temp_RF_DocumentationPGE.html"), os.path.join(htmldocspath,"RF_DocumentationPGE.html"))
    #finished
    print("...INDIVIDUAL PROCESSING FOR ALL .html FILES FINISHED")
