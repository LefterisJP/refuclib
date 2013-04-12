
""" This function checks if a module is included in the variables provided by the user

	env -- The SCONS environment variable
	module -- The Refu module name to check if it has been requested by the user
	
	Returns true for yes and false for no
"""
def moduleIncluded(env,module):
	if( module in env['REFU_MODULES']):
		return true;
	return False;


class Module:
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