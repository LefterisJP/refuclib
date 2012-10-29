import sys
import os


def getConfigValue(line,valuetype):
    """Gets the value from a configuration file variable

        --line: The line of the configuration file where the value is
        --valuetype: The name of the value to extract
    """
    part = line.partition(valuetype+"=")[2];
    part = part.lstrip(" \t");
    #part = part.partition(" ")[0];
    part = part.lstrip(" ");
    part = part.rstrip(" \r\n");
    return part;

class Config:
    """An instance containing the values set by the configuration file

        --system: The system that the scripts run in. Values are either "win32"
                  or "linux" at this point
        --refuDir: The directory of the refu library
        --builderFName: The name of the refu builder executable
        --configFName: The file name of the configuration file for this system
        --compilerDir: The directory of the compiler
        --outputName: The name that the final output should have
    """
    system=""
    refuDir=""
    builderFName=""
    configFName=""
    compilerName=""
    compilerDir=""
    outputName=""
    def __init__(self,configpath):
        """Creates a configuration file instance by reading the config file

            --configPath: The directory of the configuration file, relative to the
                          test scripts
        """
        if(sys.platform.startswith('win32')): #Windows Case
            self.builderFName = "rfb_win32.exe"
            self.configFName = "refu_config_win32"
            self.system = 'win32'
        elif(sys.platform.startswith('linux')): #linux case
            self.builderFName ="rfb_linux"
            self.configFName = "refu_config_linux"
            self.system = 'linux'
        else:    #at the moment anything else is not supported
            raise Exception("Error: Unsupported Operating System");

        #open the configuration file and read the required data
        f = open(os.path.join(configpath,self.configFName),"r");
        for line in f:
            #if it's a comment line
            if(line.startswith("#")):
                continue;
            if(line.find("COMPILER=")!=-1):
                self.compilerName = getConfigValue(line,"COMPILER")
            elif(line.find("REFU_DIR=")!=-1):
                self.refuDir = getConfigValue(line,"REFU_DIR")
                #if the current directory is indicated as the refu directory then
                if(self.refuDir == "./" or self.refuDir == "."):#get its absolute version
                    self.refuDir = os.path.abspath(configpath);
            elif(line.find("COMPILER_DIR=")!=-1):
                self.compilerDir = getConfigValue(line,"COMPILER_DIR")
            elif(line.find("OUTPUT_NAME=")!=-1):
                self.outputName = getConfigValue(line,"OUTPUT_NAME")
            elif(line.find("REFU_MODULES=")!=-1):
                modules = getConfigValue(line,"REFU_MODULES")
                if(modules != "ALL"):
                    raise Exception("Error: Refu configuration file without ALL modules selected to be compiled. Please correct this and rerun the script");





