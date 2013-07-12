import shutil
import os
from difflib import SequenceMatcher
from time import gmtime, strftime

from utils import build_msg

Import('systemAttributes refu_dir')
Import('modules env targetSystem')
#this is a list of macro defines (Except the modules and the OS define) that
#should actually go to rf_options.h
optionsList = ['_FILE_OFFSET_BITS',
               'RF_OPTION_THREADX_MSGQUEUE_SIZE',
               'RF_OPTION_FGETS_READ_BYTESN',
               'RF_OPTION_DEFAULT_ARGUMENTS',
               'RF_OPTION_ERROR_LOGGING',
               'RF_OPTION_LOCALSTACK_MEMORY_SIZE',
               '_LARGEFILE64_SOURCE']


def writeDef(file, defname):
    """ Writes a macro definition to a file

        file        -- file handle to write to
        defname     -- the macro definition string to write
    """
    file.write("#ifndef {}\n\t#define {}\n#endif\n\n".format(
        defname, defname))


def writeDefVal(file, defname, defines):
    """ Writes a macro definition with a value to a file

        file        -- file handle to write to
        defname     -- the macro definition string to write
        defines     -- a list of the defines of this building environment
                       to find out the actual value of the macro
    """
    if(defname not in defines):
        return
    value = defines[defname]
    if value is None:
        value = ""
    file.write("#ifndef {}\n\t#define {} {}\n#endif\n\n".format(
        defname, defname, value))


# -- SConscript file code START

# TODO: IF there is any other better way to check whether there were changes to
# the build settings, this should change

# create a temporary options file with the current settings
temp_options_fname = os.path.join(refu_dir, "include",
                                  "rf_options.h_temp")
options_fname = os.path.join(refu_dir, "include", "rf_options.h")
f = open(temp_options_fname, "w")
f.write(
    "/**\n"
    " ** This file contains the options that the refu library got built\n"
    " ** with by the Scons Build system.It was generated at:\n"
    " ** {} \n"
    " ** It is included from inside many files in the library so that \n"
    " ** the options can get included and that both the library and the\n"
    " ** using program can know which modules are built and which are not, \n"
    " ** along with other options\n **\n"
    " ** Note that options which are used only in the source files are \n"
    " **given to the compiler as flags during compiling and are not "
    "located here \n"
    " **\n"
    " ** Also note that this file is totally skipped due to the first \n"
    " ** ifndef if the user builds the library with scons\n"
    "**/\n\n"
    "#ifndef REFU_COMPILING //When we are compiling, some of the \n"
    "//defines should be ignored as they are given by the build"
    "system\n".format(strftime("%Y-%m-%d %H:%M:%S", gmtime())))

#depending on the OS write the proper macro
if(targetSystem == 'Windows'):
    writeDef(f, "REFU_WIN32_VERSION")
elif(targetSystem == 'Linux'):
    writeDef(f, "REFU_LINUX_VERSION")
else:
    build_msg("Unsupported Operating system detected during creating "
              "the configuration header", "Error")
    Exit(1)

# iterate through all the modules we need and report their existence
# in rf_options.h  (skipping the core module)
for mod in modules[1:]:
    writeDef(f, mod.macro)
    if mod.extra_generated is True:
        #if extra sources were generated for a module we need to know
        writeDef(f, mod.macro + "_EXTRA")

#also write all the defines which should go in the options file
for o in optionsList:
    writeDefVal(f, o, env['CPPDEFINES'])


#-- UP TO HERE WE HAVE THE DEFINES WHICH SHOULD NOT BE
#-- VISIBLE AT COMPILE TIME
f.write("#endif //closing the if compiling ifndef\n")

#also write the system attributes used in dtoa.c
f.write("#ifdef RF_DTOA_ONLY//some definitions only used in dtoa.c\n")

#define long as int if we got 64 bit longs
if(systemAttributes['longsize'] == 8):
    f.write("\t#define Long int\n")
f.write("#endif//end of dtoa.c only definitions\n")
#also give the detected endianess definition for the system at compile time
if(systemAttributes['endianess'] == 'BIG'):
    writeDef(f, 'RF_BIG_ENDIAN_COMPILE')
else:
    writeDef(f, 'RF_LITTLE_ENDIAN_COMPILE')
f.close()

# ...and now check with the original to see if we should swap them
# the reason for this check is to not always rebuild the library
newF = open(temp_options_fname)
try:
    oldF = open(options_fname)
except:
    #if the old options.h does not exist then simply rename the temporary
    os.rename(temp_options_fname, options_fname)
    newF.close()
    build_msg("rf_options.h file was created because it did not already"
              " exist", 'Info', env)
    Return('')
s = SequenceMatcher(None, newF.read(), oldF.read())
ratio = s.ratio()
newF.close()
oldF.close()

#TODO: Perhaps this way is not safe and actual line by line comparison
# would be safer. Will consider it for the future

#due to the timestamp changing we will consider them to be the same
#if there is a ratio greater than 0.99
if ratio < 0.99:
    build_msg("new rf_options.h file has a {} similarity with the old file"
              " and as such WILL be replaced".format(str(ratio)),
              'Info',
              env)
    os.remove(options_fname)
    shutil.move(temp_options_fname, options_fname)
else:
    os.remove(temp_options_fname)
    build_msg("new rf_options.h file has a {} similarity with the old file"
              " and as such WILL NOT be replaced".format(str(ratio)),
              'Info',
              env)
