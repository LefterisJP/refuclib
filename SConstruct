import os
import platform

SConscript('scripts/sconsdata/compilers.py')
SConscript('scripts/sconsdata/modules.py')


from scripts.sconsdata.modules import modules,sources
from scripts.sconsdata.compilers import compilers, setupCompiler
from scripts.sconsdata.configvariables import setupConfigVars


configFileName = 0;
sourceDir = 'src'
targetSystem = platform.system()
allowedCompilers = ['gcc', 'tcc', 'msvc']


#the command line arguments 
AddOption('--config-file', dest='configFileName',action='store', type='string',help='The name of the refu configuration file')
AddOption('--test-build', dest='testBuild',action='store_true', help='Will compile a test build')
AddOption('--compiler', dest='compiler', action='store',type='string',help='The compiler name. Allowed values are: gcc, tcc, msvc')
AddOption('--testsrc', dest='testsrc', action='store',type='string',help='When scons is ran from the automated testing framework this will be the name of the test to compile and test')


#get the compiler
compiler = GetOption('compiler');
if(compiler not in allowedCompilers):
    print "The given compiler name is not supported. Please choose one of the supported compilers. Defaulting to gcc."
    compiler = 'gcc'

#get the configuration file name from the arguments (if given)
configFileName = GetOption('configFileName')
#Create an intermediate environment to read the variables ( I don't know of any other way to read the variables sort of writing a separate variable reading python script)
vars = SConscript('scripts/sconsdata/variables.py', exports = 'configFileName')
temp = Environment(variables = vars) 

#create the building environment
#env = Environment(tools= compilers[compiler].toolsValues[targetSystem])
env = Environment()
if('test'in COMMAND_LINE_TARGETS):
    setupCompiler(compiler,env,targetSystem,temp['COMPILER_DIR'],True)
else:
    setupCompiler(compiler,env,targetSystem,temp['COMPILER_DIR'],False)

#setting needed flags, paths and defines
env.Append(CPPDEFINES = {'REFU_COMPILING': None})
if(GetOption('testBuild')):
	env.Append(CPPDEFINES = {'REFU_TEST' : None})

env.Append(CPPPATH		= os.path.join(temp['REFU_DIR'],'include'))
env.Append(CCFLAGS 		= temp['COMPILER_FLAGS'])
env.Append(LINKFLAGS	= temp['LINKER_SHARED_FLAGS'])


#now depending on each of the given modules set the compiled sources, defines and compile-time flags
if('ALL' not in temp['REFU_MODULES']):
    for mod in modules:
        if(mod.name in temp['REFU_MODULE']):
            sources.extend(mod.sources)
            env.Append(CPPDEFINES= {mod.macro:None})
            if(targetSystem == 'Windows'):
                sources.extend(mod.win32_sources)
            else:
                sources.extend(mod.linux_sources)
            #TODO More operating systems here
else:#all modules requested
    for mod in modules:
        sources.extend(mod.sources)
        env.Append(CPPDEFINES= {mod.macro:None})
        if(targetSystem == 'Windows'):
            sources.extend(mod.win32_sources)
        else:
            sources.extend(mod.linux_sources)
        #TODO More operating systems here

#setup the variables of the configuration file
setupConfigVars(temp,env)

# --  check the environment configuration by creating a configure context --
#conf = Configure(env)
# -- checks should go here
# -- configuration finished --
#env = conf.Finish()

#Perform the system check
systemAttributes = SConscript('scripts/systemcheck/systemcheck.py',exports = 'compiler')
#create the options file
SConscript('scripts/sconsdata/options.py', exports = 'modules env targetSystem systemAttributes')


outName = temp['OUTPUT_NAME']
env.VariantDir(temp['OBJ_DIR'], sourceDir, duplicate=0)
#a list comprehensions prepending the obj dir to all of the sources (instead of the sourcedir) ... Scons peculiarity 
sources = [temp['OBJ_DIR']+'/'+s for s in sources]

# -- Test non-library build --
# compiles only the test file under src/main.c
if('test' in COMMAND_LINE_TARGETS):
    #del env['CPPDEFINES']['REFU_COMPILING']
    env.Append(CPPDEFINES = {'REFU_TEST' : None})
    #compile with debuggin flags
    env.Append(CCFLAGS = '-g')
    #add the test source file
    sources.append('src/main.c')
    test = env.Program(outName, sources)
    env.Alias('test',test)
    
# -- STATIC LIBRARY
if('static' in COMMAND_LINE_TARGETS):
    env.Append(CPPDEFINES = {'REFU_STATIC_LIB' : None})
    static = env.StaticLibrary(outName, sources)
    env.Alias('static', static)

# -- SHARED LIBRARY
if('shared' in COMMAND_LINE_TARGETS):
    env.Append(CPPDEFINES = {'REFU_DYNAMIC_LIB' : None})
    shared = env.SharedLibrary(outName, sources)
    env.Alias('shared', shared)


#-- Unit Testing
if('test_shared' in COMMAND_LINE_TARGETS):
    del env['CPPDEFINES']['REFU_COMPILING']
    env.Append(LIBPATH = './Tests')
    env.Append(LIBS = outName)
    #set the rpath for GCC
    #TODO: For other compilers in Linux do something similar
    env.Append( LINKFLAGS = "-Wl,-rpath="+os.path.join(os.getcwd(),'Tests'))
    test_shared = env.Program(os.path.join('Tests','test'),os.path.join(GetOption('testsrc')))
    env.Alias('test_shared', test_shared)

if('test_static' in COMMAND_LINE_TARGETS):
    del env['CPPDEFINES']['REFU_COMPILING']
    env.Append(CPPDEFINES = {'REFU_TEST': None})
    outName = env['LIBPREFIX']+outName+env['LIBSUFFIX']
    #lib = File(os.path.join('Tests',outName))+" rt pthread"
    env.Append(LIBS = File(os.path.join('Tests',outName)) )
    test_static = env.Program(os.path.join('Tests','test'),os.path.join(GetOption('testsrc'))) #,LIBS=[lib])
    env.Alias('test_static', test_static)

#env.Default(shared)





#generate help text for the variables
Help(vars.GenerateHelpText(env))

