import os

from utils import build_msg
# -- Get the user provided options --

Import('allowedCompilers config_file code_gen')


def checkCompilerValue(key,value,environment):
    """A function to validate the compiler value"""
    if(value != None and value  not in allowedCompilers):
        print("**ERROR** The given compiler name is not supported. Please "
        "provide one of the supported compiler values")
        print " ,".join(allowedCompilers)
        Exit(-1)
    if(value == None):
        print "No compiler was given. Defaulting to GCC"
        value = 'gcc'
    return True

def sources_string_to_list(s):
    if s == "":
        build_msg("No test sources provided.", "Error")
        Exit(-1)
    sources = s.split(',')
    return sources



# Initialize the variables object
vars = Variables(config_file)

# Add Variable Options which don't have to do with the actual building of
# the library
vars.Add('__TEST_SOURCES','This is a special option used when '
         'compiling a source file for the testing framework which '
         'denotes the names of the sources of a specific test.'
         'Should be a comma separated list.',
         converter=sources_string_to_list)

vars.Add(
    BoolVariable('__TEST_BUILD','This is a special option which if passed '
                 'to scons signifies that no library will be built but a '
                 'program defined by main.c will be compiled. It\'s used to'
                 ' test the library manually. Default is false.', False))

#Add All the variable options which have to do with the building of the library
vars.Add('COMPILER', 'The compiler name. Allowed values are: gcc, tcc, msvc',
         'gcc',validator=checkCompilerValue)

vars.Add(
    PathVariable('COMPILER_DIR', 'The directory of the compiler. Will try'
                 ' to search here if scons can\'t find the requested compiler'
                 ' automatically. Should be an absolute path', '.',
                 PathVariable.PathIsDir))

vars.Add(
    PathVariable('OBJ_DIR', 'The name of the directory to use (will create'
                 ' if it does not already exist)for dumping the object files.'
                 ' Its parent directory is the refu directory', 'obj',
                 PathVariable.PathIsDirCreate))

vars.Add('COMPILER_FLAGS', 'Extra compiler flags that you want to send to '
         'the provided compiler', '')

vars.Add('LINKER_SHARED_FLAGS', 'Extra flags that would go towards the '
         'creation of the shared library', '')

vars.Add('LINKER_STATIC_FLAGS', 'Extra flags that would go towards the '
         'creation of the static library', '')

vars.Add('OUTPUT_NAME', 'The name of the output file, be it a shared or dynamic'
         ' library. Prefix or suffixed will be automatically added by '
         'SCONS where needed', 'refu')

vars.Add(
    ListVariable('REFU_MODULES', 'The modules of refu library that you want'
                 ' to build. Accepted values are either ALL or a '
                 'comma-separated list of the names of the modules you '
                 'want to compile', ['ALL'],
                 [
                     'ALL',
                     'STRING',
                     'IO', 'TEXTFILE',
                     'DATA_STRUCTURES', 
                     'INTRUSIVE_LIST',
                     'LIST', 
                     'DYNAMICARRAY',
                     'BINARY_ARRAY',
                     'HASHMAP',
                     'TIME', 'DATE',
                     'XML',
                     'THREADS', 'THREADS_SIMPLE', 'THREADS_X',
                     'SYSTEM']
             ))

vars.Add(
    ListVariable('LIST', 'These are options specific to the linked list '
                 'module. They specify what types of linked lists to '
                 'create', [], code_gen.type_dict))

vars.Add(
    ListVariable('DYNAMICARRAY',
                 'These are options specific to the dynamic array '
                 'module. They specify what types of dynamic arrays to '
                 'create', [], code_gen.type_dict))

vars.Add(
    ListVariable('HASHMAP', 'These are options specific to the hashmap '
                 'module. They specify what types of hashmaps to '
                 'create', [], code_gen.type_dict))


vars.Add(
    EnumVariable('LOG_LEVEL_DEFAULT',
                 'This option specifies the default logging level'
                 'that the log system will be using.', 'LOG_ERROR',
                 allowed_values=(
                     'LOG_EMERGENCY',
                     'LOG_ALERT',
                     'LOG_CRITICAL',
                     'LOG_ERROR',
                     'LOG_WARNING',
                     'LOG_NOTICE',
                     'LOG_INFO',
                     'LOG_DEBUG',
                 )
             )
)

vars.Add('LOG_BUFFER_SIZE', 'The initial size to allocate for the Logging '
         'system buffer', 4096)

                 
vars.Add('DEBUG', "This option determines if this will be a Debug Build (0"
         "or 1), and if more than 1 it can indicate a different debug level",
         0)
         

vars.Add(
    BoolVariable('DEFAULT_ARGUMENTS', 'If \'yes\' then the library is built'
                 ' with some functions having default arguments, utilizing '
                 'the C preprocessor. If \'no\' they are build normally. '
                 'Accepted values for this option are \'yes\' and \'no\'.'
                 , 'yes'))

vars.Add(
    BoolVariable('SAFE_MEMORY_ALLOCATION',
                 'If \'yes\' then the malloc and calloc'
                 ' calls of the library check for failure and in case of '
                 'failure log an error and exit the process.If \'no\' then '
                 'malloc and calloc are called normally.Accepted values '
                 'for this option are \'yes\' and \'no\'.'
                 ,'no' ))

vars.Add(
    BoolVariable('VERBOSE_ERRNO',
                 'If \'yes\' then a lot of the code which detects errno'
                 'being set actually logs a specific message depending on '
                 'the situation and the errno value. If \'no\' then '
                 'simply errno value is logged'
                 ,'no' ))

vars.Add('FGETS_READ_BYTESN', 'This option is the number of bytes that will '
         'be read each time by the library\'s version of fgets. Must be a '
         'positive integer number.', 512)

vars.Add('STRINGX_CAPACITY_MULTIPLIER', 'This is the multiplier by which a'
         ' StringX\'s buffer will get allocated/reallocated by when there'
         ' is a need for buffer extension. Also when the StringX gets '
         'initialized this is how much bigger than the given String the '
         'buffer will be. Must be a positive integer.', 2)

vars.Add('DYNAMICARRAY_CAPACITY_MULTIPLIER', 'This is the multiplier by which '
         'a Dynamic array\'s buffer will get allocated/reallocated by when'
         ' there is a need for buffer extension. Also when the List gets '
         'initialized this is how much bigger than the given initial size '
         'the buffer will be. Must be a positive integer.', 2)

vars.Add('LOCALSTACK_MEMORY_SIZE',
         'This is the default size in bytes of the main'
         ' thread\'s Local Stack of the Refu Library. All objects that are'
         ' initialized as temporary objects such as with the  RFS_() macro'
         ' or the RFXML_() macro are initialized in this stack. Make sure '
         'to provide a big enough value so that no overflow happens for your'
         ' program. The default value is used if '
         'no specific value is provided at rfInit()', 1048576)

vars.Add(
    BoolVariable('TEXTFILE_ADD_BOM', 'This control whether to add a '
                 'BOM(ByteOrderMark) at the beginning of newly created '
                 'TextFiles. Provides \'yes\' to add and \'no\', not to. '
                 , 'yes'))

vars.Add('THREADX_MSGQUEUE_SIZE', 'This option affects The extended thread'
         ' objects RF_ThreadX, and it denotes what should the default value'
         ' of the size of the message queue of a newly created RF_ThreadX.'
         'You still have the option to change that in run-rime through the '
         'initialization functions but if a value is not provided this will '
         'be the default value.', 10)

vars.Add('HASHMAP_LOAD_FACTOR', 'This option determines when the hashmap '
         'will rehash all of its slots. When during a hashmap insertion '
         'the ratio of occupied slots over the map\'s size is greater '
         'than this value then rehashing of the map will take place.',
         0.7)

Return('vars')