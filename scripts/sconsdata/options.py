from modules import Module


Import('systemAttributes')
Import('modules env targetSystem')
#this is a list of macro defines (Except the modules and the OS define) that should actually go to rf_options.h
optionsList = [ '_FILE_OFFSET_BITS', 'RF_OPTION_THREADX_MSGQUEUE','RF_OPTION_FGETS_READBYTESN',
                'RF_OPTION_DEFAULT_ARGUMENTS', 'RF_OPTION_VERBOSE_ERRORS', 'RF_OPTION_LOCALSTACKMEMORY_SIZE', '_LARGEFILE64_SOURCE']

""" Writes a macro definition to a file

    file        -- file handle to write to
    defname     -- the macro definition string to write
"""
def writeDef(file,defname):
    file.write("#ifndef "+defname+"\n")
    file.write("\t#define "+defname+" \n")
    file.write("#endif\n\n")
    
""" Writes a macro definition with a value to a file

    file        -- file handle to write to
    defname     -- the macro definition string to write
    defines     -- a list of the defines of this building environment to find out the actual value of the macro
"""
def writeDefVal(file,defname,defines):
    if(defname not in defines):
        return;
    value = defines[defname]
    file.write("#ifndef "+defname+"\n")
    file.write("\t#define "+defname+" ")
    if(value!= None):
        file.write(str(value))
    file.write("\n");
    file.write("#endif\n\n")


#open the file and write its introduction
f = open("../../include/rf_options.h","w");
f.write("/**\n\
** This file contains the options that the refu library got built with by the Scons Build system\n\
** It is included from inside many files in the library so that the options can\n\
** get included and that both the library and the using\n\
** program can know which modules are built and which are not, along with other options\n\
**\n\
** Note that options which are used only in the source files are given to the compiler as flags\n\
** during compiling and are not located in here\n\
**\n\
** Also note that this file is totally skipped due to the first ifndef if the user builds the library with scons\n\
**/\n\
\n\
#ifndef REFU_COMPILING //When we are compiling, some of the defines should be ignored as they are given by the build system\n");

#depending on the OS write the proper macro
if(targetSystem == 'Windows'):
    writeDef(f,"REFU_WIN32_VERSION")
elif(targetSystem == 'Linux'):
    writeDef(f,"REFU_LINUX_VERSION")
else:
    print "Error! Unsupported Operating system detected during creating the configuration header"
    exit(1)
#iterate through all the modules we need and report their existence in rf_options.h  (skipping the core module)
for mod in modules[1:]:
    writeDef(f, mod.macro)
    if mod.extra_generated == True:
        #if extra sources were generated for a module we need to know
        writeDef(f, mod.macro + "_EXTRA")
    
#also write all the defines which should go in the options file
for o in optionsList:
    writeDefVal(f,o,env['CPPDEFINES'])


#---- UP TO HERE WE HAVE THE DEFINES WHICH SHOULD NOT BE VISIBLE AT COMPILE TIME
f.write("#endif //closing the if compiling ifndef\n");    
    
#also write the system attributes used in dtoa.c
f.write("#ifdef RF_DTOA_ONLY//some definitions only used in dtoa.c\n")

#define long as int if we got 64 bit longs
if(systemAttributes['longsize']==8):
    f.write("\t#define Long int\n")
f.write("#endif//end of dtoa.c only definitions\n")
#also give the detected endianess definition for the system at compile time
if(systemAttributes['endianess']=='BIG'):
    writeDef(f,'RF_BIG_ENDIAN_COMPILE')
else:
    writeDef(f,'RF_LITTLE_ENDIAN_COMPILE')

    


f.close();
    
