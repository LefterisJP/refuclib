import os
import platform

SConscript('scripts/sconsdata/compilers.py')
SConscript('scripts/sconsdata/modules.py')


from scripts.sconsdata.modules import setup_modules
from scripts.sconsdata.compilers import setupCompiler
from scripts.sconsdata.configvariables import setupConfigVars
from scripts.sconsdata.cleanup import clean_generated_files
from scripts.sconsdata.code_gen import CodeGen

sourceDir = 'src'
targetSystem = platform.system()
allowedCompilers = ['gcc', 'tcc', 'msvc']
legalBuildTargets = [
    'shared',
    'static',
    'test_shared',
    'test_static',
    'test']

# read the configuration file variable and the extra objects
(config_file, extra_objects, refu_dir, args_before) = SConscript(
    'scripts/sconsdata/args_before_config.py')
#create the code_gen object and add all the extra objects
code_gen = CodeGen(refu_dir)
extra_sources = []
for o in extra_objects:
    extra_sources.append(code_gen.add_object(o, refu_dir))


# Create an intermediate environment to read the build variables
vars = SConscript('scripts/sconsdata/variables.py',
                  exports='allowedCompilers config_file code_gen')
temp = Environment(variables=vars)

# create the building environment and setup the compiler
env = Environment()
setupCompiler(env, targetSystem, temp)

#setting needed flags, paths and defines
env.Append(CPPDEFINES={'REFU_COMPILING': None})
if(temp['__TEST_BUILD']):
    env.Append(CPPDEFINES={'REFU_TEST': None})

env.Append(CPPPATH=os.path.join(refu_dir, 'include'))
env.Append(CCFLAGS=temp['COMPILER_FLAGS'])
env.Append(LINKFLAGS=temp['LINKER_SHARED_FLAGS'])


#TODO: Fix the "only if actually building sections" that are here
#  to differentiate when building the library and when building the tests.
# Find a nice and clean way to have those separate and not
#  affecting one another

#only if actually building setup the required modules
if 'shared' in COMMAND_LINE_TARGETS or 'static' in COMMAND_LINE_TARGETS:
    (modules, sources) = setup_modules(temp, env, targetSystem,
                                       refu_dir, code_gen)
    #also add the extra sources (if any) given by the extra objects
    sources.extend(extra_sources)

#setup the variables of the configuration file
setupConfigVars(temp, env)

#Perform the system check
compiler = temp['COMPILER']
systemAttributes = SConscript('scripts/systemcheck/systemcheck.py',
                              exports='compiler')
#only if actually building create the options file
if 'shared' in COMMAND_LINE_TARGETS or 'static' in COMMAND_LINE_TARGETS:
    SConscript('scripts/sconsdata/options.py',
               exports='modules env targetSystem systemAttributes refu_dir')

outName = temp['OUTPUT_NAME']

#only if actually building set the obj dir and prepend obj dir to all sources
if 'shared' in COMMAND_LINE_TARGETS or 'static' in COMMAND_LINE_TARGETS:
    env.VariantDir(temp['OBJ_DIR'], sourceDir, duplicate=0)
    #a list comprehension prepending the obj dir to all of the sources
    # (instead of the sourcedir) ... Scons peculiarity
    sources = [temp['OBJ_DIR']+'/'+s for s in sources]

# -- From here and on check build targets
# Check if there is no build target given
if len(COMMAND_LINE_TARGETS) == 0 and not env.GetOption('clean'):
    print ("**MESSAGE** No build target was specified so the Refu SCons "
           "Building script has nothing to build. Please specify one of"
           " the legal build targets \'shared\' and \'static\' "
           "via command line")

#Check if there is an illegal build target given
for givenTarget in COMMAND_LINE_TARGETS:
    if givenTarget not in legalBuildTargets:
        print ("***ERROR*** Provided build target \"{}\" is not a legal"
               " target for Refu Library. Quitting Build Script"
               " ...".format(givenTarget))
        Exit(-1)

#If clean is specified make sure that we delete all of the generated files
if env.GetOption('clean'):
    clean_generated_files(refu_dir, code_gen)

# -- Test non-library build --
# compiles only the test file under src/main.c
if 'test' in COMMAND_LINE_TARGETS:
    env.Append(CPPDEFINES={'REFU_TEST': None})
    #compile with debuggin flags
    env.Append(CCFLAGS='-g')
    #add the test source file
    sources.append('src/main.c')
    test = env.Program(outName, sources)
    env.Alias('test', test)

# -- STATIC LIBRARY
if 'static' in COMMAND_LINE_TARGETS:
    env.Append(CPPDEFINES={'REFU_STATIC_LIB': None})
    static = env.StaticLibrary(outName, sources)
    env.Alias('static', static)

# -- SHARED LIBRARY
if 'shared' in COMMAND_LINE_TARGETS:
    env.Append(CPPDEFINES={'REFU_DYNAMIC_LIB': None})
    shared = env.SharedLibrary(outName, sources)
    env.Alias('shared', shared)


#-- Unit Testing
if 'test_shared' in COMMAND_LINE_TARGETS:
    del env['CPPDEFINES']['REFU_COMPILING']
    env.Append(LIBPATH='./Tests')
    env.Append(LIBS=outName)
    # add debugging symbols to the tests
    env.Append(CCFLAGS='-g')
    #set the rpath for GCC
    #TODO: For other compilers in Linux do something similar
    env.Append(LINKFLAGS="-Wl,-rpath="+os.path.join(os.getcwd(),
                                                    'Tests'))
    test_shared = env.Program(os.path.join('Tests', 'test'),
                              os.path.join(temp['__TEST_SOURCE']))
    env.Alias('test_shared', test_shared)

if 'test_static' in COMMAND_LINE_TARGETS:
    del env['CPPDEFINES']['REFU_COMPILING']
    env.Append(CPPDEFINES={'REFU_TEST': None})
    # add debugging symbols to the tests
    env.Append(CCFLAGS='-g')
    outName = env['LIBPREFIX']+outName+env['LIBSUFFIX']
    env.Append(LIBS=File(os.path.join('Tests', outName)))
    test_static = env.Program(os.path.join('Tests', 'test'),
                              os.path.join(temp['__TEST_SOURCE']))
    env.Alias('test_static', test_static)
#generate help text for the variables
Help(vars.GenerateHelpText(env))
Help(args_before.GenerateHelpText(env))
