import os
from code_gen import code_gen

class Module:
    """Represents a module of the code with its names, its list of sources
       its #ifdef macro and a list of other modules it depends on
    """
    name = ""
    sources = []
    win32_sources = []
    linux_source = []
    macro = []
    dependencies = []
    can_generate = ""
    gen_name_sub = []
    extra_generated = False
    def __init__(self,name,sources=[], macro="", win32_sources=[],
                 linux_sources=[], dependencies=[], can_generate="",
                 gen_name_sub=[]):
            self.name = name
            self.sources = sources
            self.macro = macro
            self.win32_sources = win32_sources
            self.linux_sources = linux_sources
            self.dependencies = dependencies
            self.can_generate = can_generate
            self.gen_name_sub = gen_name_sub
            self.extra_generated = False

    def add(self, sources, env, targetSystem, arg_env,
            deps=None, check=True):
        """Addition of a module's sources including
           additional check for dependencies and 
           recursive addition of said dependencies

           -sources: The list of sources for compilation
           -env: The Scons environment variable
v           -targetSystem: The OS name
           -deps: A list of current modules to build -- helps in
            avoiding duplication
           -check: A flag to determine whether we should check for
            dependencies or not
        """
        sources.extend(self.sources)
        env.Append(CPPDEFINES= {self.macro:None})
        if check:
            needed = set(self.dependencies)-set(deps)
            for m in modules:
                    if m.name in needed:
                            deps.append(m.name)
                            m.add(sources,env,targetSystem,deps)
        if(targetSystem == 'Windows'):
                sources.extend(self.win32_sources)
        else:
                sources.extend(self.linux_sources)
        # from here and under we are checking if more sources should be 
        # generated
        if(self.can_generate != ""):
            #if any additional data types are requested for this module
            if arg_env[self.can_generate] != []:
                #take the name from the first source for now
                fullname = self.sources[0]
                name = os.path.basename(fullname)
                #get rid of .c
                name = name[:len(name)-2]
                parent_dir = os.path.dirname(fullname)
                root = arg_env['REFU_DIR']
                #for every type of data given for the module
                for d_type in arg_env[self.can_generate]:
                    #generate the requested extra sources for each
                    code_gen(name, parent_dir, root,
                                   d_type, self.gen_name_sub)
                    #and of course add it to the sources to compile
                    sources.append(
                        os.path.join(parent_dir, name + d_type + ".c")
                    )
                #add lines to rftokens.h
                #TODO
                #create the extra include file
                create_includes(name, parent_dir,
                                root,  arg_env[self.can_generate])
                #add the appropriate define to include
                #the extra headers for the source we generated
                env.Append(CPPDEFINES= {self.macro+"_EXTRA": None})
                #also mark this module as needing the extra macro defined
                self.extra_generated = True

                                            

def create_includes(name, parent_dir, root, types_list):
    f = open(
        os.path.join(root, "include", parent_dir, name + "_extra.h"),
        "w")
    for type_symbol in types_list:
        f.write("#include<{}/{}_decl.h>\n".format(parent_dir,
                                                  name + type_symbol))
        f.write("#include<{}/{}.h>\n".format(parent_dir,
                                                  name + type_symbol))
    f.close()

def setup_modules(arg_env, env, targetSystem):
    """
       Depending on the given modules this function shall setup
       the required source file and dependencies
    """
    if 'ALL' not in arg_env['REFU_MODULES']:
        # add the core modules 
        if 'CORE' not in arg_env['REFU_MODULES']:
            arg_env['REFU_MODULES'].append('CORE')
            if 'SYSTEM' not in arg_env['REFU_MODULES']:
                arg_env['REFU_MODULES'].append('SYSTEM')
                #make a copy of the dependencies
                deps = arg_env['REFU_MODULES'][:]
                for mod in modules:
                    if mod.name in arg_env['REFU_MODULES']:
                        mod.add(sources, env, targetSystem,
                                arg_env, deps)
    else:  # all modules requested
        for mod in modules:
            mod.add(sources, env, targetSystem,
                           arg_env, check=False)
    return (modules, sources)

#empty list of modules and sources
sources			= []
modules			= []

#add all the modules one by one
modules.append(
Module("CORE",
['refu.c', 'Utils/endianess.c', 'Utils/error.c', 'Utils/localmem.c',
 'Math/rf_math.c', 'Numeric/Float/dtoa.c',
 'Numeric/Integer/conversion.c'])
)

modules.append(
Module("STRING",
['String/common.c', 'String/commonp.c', 'String/conversion.c', 'String/core.c', 'String/corex.c', 'String/files.c', 'String/filesx.c', 'String/format.c', 'String/manipulation.c', 'String/manipulationx.c', 'String/retrieval.c', 'String/traversalx.c', 'String/unicode.c'],
macro			= "RF_MODULE_STRINGS",
dependencies	= ['IO'])
)

modules.append(
Module("IO",
['IO/buffer.c', 'IO/file.c', 'IO/printf.c'],
macro			= "RF_MODULE_IO",
win32_sources	= ['IO/io_win32.c'],
linux_sources	= ['IO/io_linux.c'])
)

modules.append(
Module("TEXTFILE", 
['IO/textfile.c', 'IO/textfilep.c'],
macro 			= "RF_MODULE_IO_TEXTFILE",
dependencies	= ['STRING'])
)

modules.append(
Module("LINKED_LIST",
['Data_Structures/linkedlist.c'],
macro = "RF_MODULE_DS_LINKED_LIST",
can_generate = "LINKED_LIST",
gen_name_sub = ["RF_LL_Node",
                "RF_LinkedList",
                "rfLinkedList",
                "RF_LINKEDLIST",
                "linkedlist"
               ]
))

modules.append(
Module("LIST",
['Data_Structures/list.c'],
macro			= "RF_MODULE_DS_LIST")
)

modules.append(
Module("ARRAY",
['Data_Structures/array.c'],
macro			= "RF_MODULE_DS_ARRAY",
dependencies 	= ['BINARY_ARRAY'])
)

modules.append(
Module("BINARY_ARRAY",
['Data_Structures/binaryarray.c'],
macro			= "RF_MODULE_DS_BARRAY")
)

modules.append(
Module("TIME",
win32_sources	= ['Time/time_win32.c'],
linux_sources	= ['Time/time_linux.c'],
macro			= "RF_MODULE_TIME_TIMER")
)

modules.append(
Module("DATE",
['Time/date.c'],
macro			= "RF_MODULE_TIME_DATE")
)

modules.append(
Module("XML",
['Data_Formats/xml.c', 'Data_Formats/xmltag.c', 'Data_Formats/commonp.c'],
macro			= "RF_MODULE_DF_XML",
dependencies	= ['TEXTFILE', 'String', 'List'])
)

modules.append(
Module("THREADS",
win32_sources 	= ['Threads/thread_win32.c', 'Threads/mutex_win32.c', 'Threads/semaphore_win32.c'],
linux_sources	= ['Threads/thread_linux.c', 'Threads/mutex_linux.c', 'Threads/semaphore_linux.c'],
macro			= "RF_MODULE_THREAD")
)

modules.append(Module("THREADSX",
win32_sources	= ['Threads/threadx_win32.c'],
linux_sources	= ['Threads/threadx_linux.c'],
macro			= "RF_MODULE_THREADX")
)


modules.append(Module("SYSTEM",
['System/sysinfop.c'],
win32_sources	= ['System/system_win32.c'],
linux_sources	= ['System/system_linux.c'],
macro			= "RF_MODULE_SYSTEM")
)

