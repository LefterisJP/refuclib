import xml.etree.ElementTree as XML
import os


def getTag(compilerName,root,name,system,exclusive):
    """Returns the contents of the expected tag. If not found it raises an exception

        If the tag has a system specific subtag it also appens their contents
        into the return string.

        Arguments:
            --compilerName: The name of the compiler. Used for logging an error
                            in case things go bad
            --root: The root of the xml tree.
            --name: The name of the tag we are searching for in the tree
            --system: The name of the system that the checks run in. Used to
                      check for existence of system-specific tag
            --exclusive: If this flag is true and the tag has a system specific
                         subtag it also appends their contents into the return string.

    """
    ret = ""
    #check if this tag exists under the parent
    node = root.find(name);
    if(node == None):#if not then raise an exception
        raise Exception("Expected tag {0} was not found in {1} compiler's data file");
    ret = node.text;
    ret = ret.rstrip("\n\t")#strip of any extra chars it might have on the right we don't want
    #check for a system specific subtag
    child = node.find(system);
    if(child != None):
        if(exclusive):
            ret = child.text
        else:
            ret = ret+" "+child.text;

    #by here we will have gotten the correct tag so return it
    return ret




class Compiler:
    """Each compiler object represents a different compiler

        The attributes of each compiler are:
        --name: The unique name value identifying a compiler
        --compilerDir:  The directory of the compiler in this system
        --compilerExec: The executable of the compiler
        --macroArg: The program argument with which you can pass macro flags to
                    the compiler
        --compileCMD: The command to be used by this compiler to compile a test
                      file into an executable
        --flags: Any flags to pass to the compiler during compiling a test
        --dynamicExtension: A string containing the extension used for a dynamic library
                           by the system
        --staticExtension: A string containing the extensions used for a static
                           library by the system
    """
    name= ""
    compilerDir=""
    compilerExec=""
    macroArg=""
    cmd=""
    flags=""
    dynamicExtension=""
    staticExtension=""


    def __init__(self,config):
        """The constructor of a compiler object

            --config: The configuration file instance where we must have already read
                   all the relevant values
        """
        #open the corresponding compiler xml file
        self.name = config.compilerName;
        self.compilerDir = config.compilerDir;
        xml = XML.parse(os.path.join(config.refuDir,"Data","Compilers",self.name.lower()+".xml"))
        root = xml.getroot()
        #get the required arguments, if anything is not found an exception is raised
        self.macroArg = getTag(self.name,root,"macroargument",config.system,True)

        #checking compiler arguments
        node = root.find("compiler");
        if(node == None):#if not then raise an exception
            raise Exception("Expected tag <compiler> was not found in {0} compiler's data file".format(self.name));
        self.compilerExec = getTag(self.name,node,"name",config.system,True)
        if(config.system =="win32"):
            self.compilerExec = self.compilerExec+".exe"
        self.cmd = getTag(self.name,node,"test_cmd",config.system,True)
        #get the tests compile flags if any
        try:
            self.flags = getTag(self.name,node,"test_flags",config.system,False)
        except Exception as err:
            self.flags = ""
        #check the dynamic linker for the output extension
        root = root.find("linker")
        node = root.find("dynamic_linker")
        if(node == None):
            raise Exception("Expected tag <dynamic_linker> was not found in {0} compiler's data file".format(self.name));
        cmd = getTag(self.name,node,"cmd",config.system,True);
        cmd = cmd.partition("$OUTPUT")[2];
        part = cmd.partition(" ");
        if(part[1]!=""):
            cmd = part[0];
        self.dynamicExtension = cmd;
        #check the static linker for the output extension
        node = root.find("static_linker")
        if(node == None):
            raise Exception("Expected tag <static_linker> was not found in {0} compiler's data file".format(self.name));
        cmd = getTag(self.name,node,"cmd",config.system,True);
        cmd = cmd.partition("$OUTPUT")[2];
        part = cmd.partition(" ");
        if(part[1]!=""):
            cmd = part[0];
        self.staticExtension = cmd;


