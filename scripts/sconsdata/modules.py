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
	def __init__(self,name,sources=[],macro="",win32_sources=[],linux_sources=[],dependencies=[]):
		self.name = name
		self.sources = sources
		self.macro = macro
		self.win32_sources = win32_sources
		self.linux_sources = linux_sources
		self.dependencies = dependencies

        def add(self, sources, env, targetSystem, deps):
            """Addition of a module's sources including
               additional check for dependencies and 
               recursive addition of said dependencies

               -sources: The list of sources for compilation
               -env: The Scons environment variable
               -targetSystem: The OS name
               -deps: A list of current modules to build -- helps in
                avoiding duplication
            """
            sources.extend(self.sources)
            env.Append(CPPDEFINES= {self.macro:None})
            needed = set(self.dependencies)-set(deps)
            for m in modules:
                    if m.name in needed:
                            deps.append(m.name)
                            m.add(sources,env,targetSystem,deps)
            if(targetSystem == 'Windows'):
                    sources.extend(self.win32_sources)
            else:
                    sources.extend(self.linux_sources)


        def simple_add(self, sources, env, targetSystem):
            """Simple addition of a module's sources without checking
               dependencies.

               -sources: The list of sources for compilation
               -env: The Scons environment variable
               -targetSystem: The OS name
            """
            sources.extend(self.sources)
            env.Append(CPPDEFINES= {self.macro:None})
            if(targetSystem == 'Windows'):
                    sources.extend(self.win32_sources)
            else:
                    sources.extend(self.linux_sources)

#empty list of modules and sources
sources			= []
modules			= []

#add all the modules one by one
modules.append(
Module("CORE",
['refu.c', 'Utils/endianess.c', 'Utils/error.c', 'Utils/localmem.c', 'Math/rf_math.c', 'Numeric/Float/dtoa.c', 'Numeric/Integer/conversion.c'])
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