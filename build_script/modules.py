import os
from code_gen import CodeGen
from utils import build_msg

from templates import Template, TemplateGeneratedStructure

class TemplateSourcesError(Exception):
    def __init__(self, module_name):
        self.module_name = module_name
    def __str__(self):
        return ("Template module \"{}\" contains {} no non-template"
                "source files and generic data structure has been "
                "requested".format(self.module_name))

class Module:
    """Represents a module of the code with its names, its list of sources
       its #ifdef macro and a list of other modules it depends on
    """
    def __init__(self,name,sources=[], macro="", win32_sources=[],
                 linux_sources=[], dependencies=[], has_template="",
                 template_files={}, gen_name_sub=[]):
            self.name = name
            self.sources = sources
            self.macro = macro
            self.win32_sources = win32_sources
            self.linux_sources = linux_sources
            self.dependencies = dependencies
            self.has_template = has_template
            self.template_files = template_files
            self.gen_name_sub = gen_name_sub
            self.extra_generated = False


    def add(self, sources, env, targetSystem, arg_env, refu_dir,
            deps=[], check=True, codegen=None):
        """Addition of a module's sources including
           additional check for dependencies and 
           recursive addition of said dependencies

           -sources: The list of sources for compilation
           -arg_env: The Scons environment variable for arguments
           -refu_dir: The root directory of the library
           -targetSystem: The OS name
           -deps: A list of current modules to build -- helps in
            avoiding duplication
           -check: A flag to determine whether we should check for
            dependencies or not
           -codegen: A CodeGen object
        """
        env.Append(CPPDEFINES= {self.macro:None})
        #if this is a template module
        if(self.has_template != ""):
            #if any data types are requested for this module
            if arg_env[self.has_template] != []:
                template = Template(self.name, self.template_files, 
                                    arg_env[self.has_template],
                                    codegen.obj_dict
                )
                #for every type of data given for the module
                for d_type in arg_env[self.has_template]:
                    #if the generic type is requested, add the source files
                    if d_type == "generic":
                        if self.sources == []:
                            raise TemplateSourcesError(self.name)
                        sources.extend(self.sources)
                        continue
                        
                    #generate the requested extra sources for each
                    generated_sources = []
                    for _, structure in template.structures[d_type].iteritems():
                        generated_sources.append(codegen.code_gen(
                            template, structure, refu_dir,
                            d_type, self.gen_name_sub
                        ))

                    # add it to the sources to compile if it's a library type
                    # if not it's the user's responsibility to copy the generated
                    # .c file into his project
                    if d_type in codegen.library_types:
                        sources.extend(generated_sources)

                #TODO: add lines to rftokens.h
                #create the extra include file
                create_includes(template, refu_dir)
                #add the appropriate define to include
                #the extra headers for the source we generated
                env.Append(CPPDEFINES= {self.macro+"_EXTRA": None})
                #also mark this module as needing the extra macro defined
                self.extra_generated = True
        else:
            #else if this is a normal - non template module
            sources.extend(self.sources)
            if(targetSystem == 'Windows'):
                sources.extend(self.win32_sources)
            else:
                sources.extend(self.linux_sources)
            if check:
                needed = set(self.dependencies)-set(deps)
                for m in modules:
                    if m.name in needed:
                        deps.append(m.name)
                        m.add(sources, env, targetSystem, arg_env,
                              refu_dir, deps, True, codegen)

def create_includes(template, root):
    """
        Creates the include files needed for each of the generated
        source files.
    """
    f = open(
        os.path.join(
            root, "include", template.parent_dir, template.name + "_extra.h"), "w")
    for structure in template.iterate():
        f.write("#include <{}/{}_decl.h>\n".format(
            template.parent_dir, structure.header_prefix
        ))
        f.write("#include <{}/{}.h>\n".format(
            template.parent_dir, structure.header_prefix
        ))
    f.close()

def setup_modules(arg_env, env, targetSystem, refu_dir, code_gen):
    """
       This is the function that is called from the main SConstruct file.
       Depending on the given modules this function shall setup
       the required source file and dependencies
       --arg_env: The environment containing the arguments given to the
         build system
       --env: The build environment
       --targetSystem: The architecture of the target system
       --refu_dir: Root directory of the refu library
       --code_gen: A CodeGen object that must have already been initialized
    """
    if 'ALL' not in arg_env['REFU_MODULES']:
        # add the core and system modules 
        if 'CORE' not in arg_env['REFU_MODULES']:
            arg_env['REFU_MODULES'].append('CORE')
            if 'SYSTEM' not in arg_env['REFU_MODULES']:
                arg_env['REFU_MODULES'].append('SYSTEM')
                #make a copy of the dependencies
                deps = arg_env['REFU_MODULES'][:]
                for mod in modules:
                    if mod.name in arg_env['REFU_MODULES']:
                        mod.add(sources, env, targetSystem,
                                arg_env, refu_dir, deps, codegen=code_gen)
    else:  # all modules requested
        for mod in modules:
            mod.add(sources, env, targetSystem,
                    arg_env, refu_dir, check=False, codegen=code_gen)
    return (modules, sources)

#empty list of modules and sources
sources			= []
modules			= []

#add all the modules one by one
modules.append(
    Module("CORE",
           ['refu.c', 'Utils/endianess.c', 'Utils/error.c',
            'Utils/localmem.c', 'Math/rf_math.c', 'Numeric/Float/dtoa.c',
            'Numeric/Integer/conversion.c'])
)

modules.append(
    Module("STRING",
           ['String/common.c', 'String/commonp.c', 'String/conversion.c',
            'String/core.c', 'String/corex.c', 'String/files.c',
            'String/filesx.c', 'String/format.c', 'String/manipulation.c',
            'String/manipulationx.c', 'String/retrieval.c',
            'String/traversalx.c', 'String/unicode.c'],
           macro = "RF_MODULE_STRINGS",
           dependencies	= ['IO'])
)

modules.append(
    Module("IO",
           ['IO/buffer.c', 'IO/file.c', 'IO/printf.c'],
           macro = "RF_MODULE_IO",
           win32_sources = ['IO/io_win32.c'],
           linux_sources = ['IO/io_linux.c'])
)

modules.append(
    Module("TEXTFILE", 
           ['IO/textfile.c'],
           macro = "RF_MODULE_IO_TEXTFILE",
           dependencies	= ['STRING'])
)


modules.append(
    Module("INTRUSIVE_LIST",
           ['Data_Structures/intrusive_list.c'],
           macro = "RF_MODULE_INTRUSIVE_LIST",
       )
)

modules.append(
    Module("LIST",
           [],
           macro = "RF_MODULE_LIST",
           has_template = "LIST",
           template_files = {"simple": "Data_Structures/list.ctemplate"},
           gen_name_sub = ["RF_L_Node",
                           "RF_List",
                           "rfList",
                           "RF_LIST",
                           "list"]
       )
)


modules.append(
    Module("DYNAMICARRAY",
           [],
           macro = "RF_MODULE_DYNAMICARRAY",
           has_template = "DYNAMICARRAY",
           template_files = {"simple": "Data_Structures/dynamicarray.ctemplate"},
           gen_name_sub = ["RF_DynamicArray",
                           "rfDynamicArray",
                           "RF_DYNAMICARRAY",
                           "dynamicarray"]
       )
)

modules.append(
    Module("HASHMAP",
           ['Data_Structures/hashmap.c'],
           macro = "RF_MODULE_HASHMAP",
           has_template = "HASHMAP",
           template_files = {"shallow": "Data_Structures/hashmap_shallow.ctemplate",
                             "deep": "Data_Structures/hashmap_deep.ctemplate"},
           gen_name_sub = ["RF_Hashmap",
                           "rfHashmap",
                           "RF_HASHMAP",
                           "hashmap",
                           "RF_Hashslot"
                       ]
       )
)

modules.append(
    Module("BINARY_ARRAY",
           ['Data_Structures/binaryarray.c'],
           macro = "RF_MODULE_BINARYARRAY")
)

modules.append(
    Module("TIME",
           win32_sources = ['Time/time_win32.c'],
           linux_sources = ['Time/time_linux.c'],
           macro = "RF_MODULE_TIME_TIMER")
)

modules.append(
    Module("DATE",
           ['Time/date.c'],
           macro			= "RF_MODULE_TIME_DATE")
)

modules.append(
    Module("XML",
           #['Data_Formats/xml.c', 'Data_Formats/xmltag.c', 'Data_Formats/commonp.c'],
           macro			= "RF_MODULE_DF_XML",
           dependencies	= ['TEXTFILE', 'String', 'List'])
)

modules.append(
    Module("THREADS",
           win32_sources = ['Threads/thread_win32.c',
                            'Threads/mutex_win32.c',
                            'Threads/semaphore_win32.c'],
           linux_sources = ['Threads/thread_linux.c',
                            'Threads/mutex_linux.c',
                            'Threads/semaphore_linux.c'],
           macro = "RF_MODULE_THREAD")
)

modules.append(
    Module("SYSTEM",
           ['System/sysinfop.c'],
           win32_sources = ['System/system_win32.c'],
           linux_sources = ['System/system_linux.c'],
           macro = "RF_MODULE_SYSTEM")
)

