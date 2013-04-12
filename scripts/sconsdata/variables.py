# -- Get the user provided options --

Import('configFileName')
if(configFileName == None):
	vars = Variables('refu.config')
else: 
	vars = Variables(configFileName)


vars.Add(PathVariable('COMPILER_DIR', 'The directory of the compiler. Will try to search here if scons can\'t find the requested compiler automatically. Should be an absolute path', '.',PathVariable.PathIsDir))

vars.Add(PathVariable('REFU_DIR', 'The root directory of the refu library. Absolute value', '.',PathVariable.PathIsDir))

vars.Add(PathVariable('OBJ_DIR', 'The name of the directory to use (will create if it does not already exist)for dumping the object files. Its parent directory is the refu directory', 'obj',PathVariable.PathIsDirCreate))

vars.Add('COMPILER_FLAGS', 'Extra compiler flags that you want to send to the provided compiler', '')

vars.Add('LINKER_SHARED_FLAGS', 'Extra flags that would go towards the creation of the shared library', '')

vars.Add('LINKER_STATIC_FLAGS', 'Extra flags that would go towards the creation of the static library', '')

vars.Add('OUTPUT_NAME', 'The name of the output file, be it a shared or dynamic library. Prefix or suffixed will be automatically added by SCONS where needed', 'refu')

vars.Add(ListVariable('REFU_MODULES', 'The modules that you want to build of refu library. Accepted values are either ALL or a comma-separated list of the names of the modules you want to compile', ['ALL'],
['ALL',
'STRING',
'IO', 'TEXTFILE',
'DATA_STRUCTURES', 'LIST', 'ARRAY', 'BINARY_ARRAY',
'TIME', 'DATE',
'XML',
'THREADS', 'THREADS_SIMPLE', 'THREADS_X',
'SYSTEM']
))

vars.Add(BoolVariable('DEFAULT_ARGUMENTS', 'If \'yes\' then the library is built with some functions having default arguments, utilizing the C preprocessor. If \'no\' they are build normally. Accepted values for this option are \'yes\' and \'no\'. Default is \'yes\'.', 1))

vars.Add(BoolVariable('SAFE_MEMORY_ALLOC', 'If \'yes\' then the malloc and calloc calls of the library check for failure and in case of failure log an error and exit the process.If \'no\' then malloc and calloc are called normally.Accepted values for this option are \'yes\' and \'no\'. Default is \'no\'.', 0))

vars.Add(BoolVariable('VERBOSE_ERROR_LOGGING', 'If \'yes\' then the library\'s error logging will be verbose which basically means that every error will be printed in the appropriate log file stream. Default is \'yes\'.', 1))

vars.Add('FGETS_READ_BYTESN', 'This option is the number of bytes that will be read each time by the library\'s version of fgets. Must be a positive integer number. Default is 512', 512)

vars.Add('STRINGX_CAPACITY_MULTIPLIER', 'This is the multiplier by which a StringX\'s buffer will get allocated/reallocated by when there is a need for buffer extension. Also when the StringX gets initialized this is how much bigger than the given String the buffer will be. Must be a positive integer. Default is 2', 2)

vars.Add('LIST_CAPACITY_MULTIPLIER', 'This is the multiplier by which a List\'s buffer will get allocated/reallocated by when there is a need for buffer extension. Also when the List gets initialized this is how much bigger than the given initial size the buffer will be. Must be a positive integer. Default is 2', 2)

vars.Add('LOCAL_STACK_SIZE', 'This is the default size in bytes of the main thread\'s Local Stack of the Refu Library. All objects that are initialized as temporary objects such as with the  RFS_() macro or the RFXML_() macro are initialized in this stack. Make sure to provide a big enough value so that no overflow happens for your program. Default Value is around 1 MB. This is just the value to be used if no specific value is provided at rfInit()', 1048576)

vars.Add(BoolVariable('TEXTFILE_ADD_BOM', 'This control whether to add a BOM(ByteOrderMark) at the beginning of newly created TextFiles. Provides \'yes\' to add and \'no\', not to. Default is \'yes\'', 1))

vars.Add('THREADX_MSGQUEUE_SIZE', 'This option affects The extended thread objects RF_ThreadX, and it denotes what should the default value of the size of the message queue of a newly created RF_ThreadX. You still have the option to change that in run-rime through the initialization functions but if a value is not provided this will be the default value.Default value is 10', 10)

Return('vars')
