from utils import build_msg


class Compiler:
    name = ""
    path = ""
    bin = ""
    dlink = ""
    slink = ""
    toolsValues = {}
    coptions = {}
    cflags = {}
    lflags = {}
    debug_flags = {}

    def __init__(self, name, path=0, bin=0, dlink=0, slink=0, coptions=0,
                 cflags=0, lflags=0, libs=0, toolsValues=0, debug_flags=0):
        self.name = name
        self.path = path
        self.bin = bin
        self.dlink = slink
        self.cflags = cflags
        self.lflags = lflags
        self.libs = libs
        self.toolsValues = toolsValues
        self.coptions = coptions
        self.debug_flags = debug_flags

#Defining the compilers
compilers = {}
#GCC
compilers['gcc'] = Compiler(
    name='gcc',
    coptions={'all': ['-static-libgcc',
                      '-std=gnu99' #gnu99 for c99 + gcc extensions (e.g: typeof)
                      ], 
              'Windows': [],
              'Linux': []},
    cflags={'all': {}, 'Windows': {}, 'Linux': {'_GNU_SOURCE': None}},
    lflags={'all': [], 'Windows': [], 'Linux': []},
    libs={'all': [], 'Windows': [], 'Linux': ['rt', 'pthread', 'm']},
    toolsValues={'Windows': ['mingw'], 'Linux': ['gcc']},
    debug_flags={'all': ["-g"], 'Windows': [], 'Linux': []}
)


def add_compiler_field(env, os, compiler_name, attribute, compiler_field):
    """
       Adds a particular compiler options field  to the build environment
       Depending on the build environment attribute that this affects
       the function either updates a dictionary or extends a list.

       --env: The build environment to extend/update
       --os: The operating system the library compiles in
       --compiler_name: The name of the compiler
       --attribute: The environment attribute we are changing. For example
         'CCFLAGS' or 'CPPDEFINES'
       --compiler_field: The compiler field we are using. For example
         'debug_flags' or 'libs'
    """
    cdict = compilers[compiler_name].__dict__
    l = ['all', os]
    for v in l:
        if cdict[compiler_field][v] != []:
            if isinstance(env[attribute], dict):
                env[attribute].update(cdict[compiler_field][v])
            else:
                env[attribute].extend(cdict[compiler_field][v])


def setupCompiler(env, os, arg_env):
    """
        Setups the compiler depending on the system
        --env: The SCONS environment
        --os: Operating system value
        --arg_env: The environment of the given build arguments
    """
    compilerdir = arg_env['COMPILER_DIR']
    compiler_name = arg_env['COMPILER']
    #add general options and defines for the refu project
    if(os == 'Windows'):
        env.Append(CPPDEFINES={'REFU_WIN32_VERSION': None})
        env.Append(CPPDEFINES={'_WIN32_WINNT': '0x501'})
    elif(os == 'Linux'):
        env.Append(CPPDEFINES={'REFU_LINUX_VERSION': None})
        env.Append(CPPDEFINES={'_LARGEFILE64_SOURCE': None})
    else:
        build_msg("Unsuported Operating System value \"{}\" "
                  "Detected...Quitting".format(os), "Error")
        Exit(1)
    env.Append(CPPDEFINES={'_FILE_OFFSET_BITS': 64})

    #add debug symbols to the compiler if Debug is not 0
    if arg_env['DEBUG'] != 0:
        add_compiler_field(env, os, compiler_name, 'CCFLAGS', 'debug_flags')
        env.Append(CPPDEFINES={'RF_OPTION_DEBUG': None})
    else:
        env.Append(CPPDEFINES={'NDEBUG': None})

    # figure out the tools value
    env.Replace(tools=compilers[compiler_name].toolsValues[os])
    # if a compiler dir has been given then use that.
    # Not given is the '.' directory
    if compilerdir != '.':
        build_msg("Using \"{}\" as the compiler directory as instructed"
                  "".format(compilerdir), 'Info', env)
        env.Replace(ENV={'PATH': compilerdir})

    #set compiler defines, and compile and link options
    add_compiler_field(env, os, compiler_name, 'CCFLAGS', 'coptions')
    add_compiler_field(env, os, compiler_name, 'CPPDEFINES', 'cflags')
    # it is possible that env['LIBS'] does not exist yet here and since
    # inside add_compiler_field I am not using env.Append() I need to make
    # sure that there are no key errors by doing this:
    env['LIBS'] = []
    add_compiler_field(env, os, compiler_name, 'LIBS', 'libs')
