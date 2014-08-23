Import('env')

local_env = env.Clone()

# setup the required modules
(modules, orig_sources) = SConscript(
    '#build_system/build_extra/clib/modules.py',
    exports='local_env')

# only if actually building create the options file
options_header = SConscript(
    '#build_system/build_extra/clib/options_header_creator.py',
    exports='modules local_env')

outName = local_env['OUTPUT_NAME']

# set the obj dir and prepend obj dir to all sources
local_env.VariantDir(local_env['OBJ_DIR'], "src", duplicate=0)
# a list comprehension prepending the obj dir to all of the sources
# (instead of the sourcedir) ... Scons peculiarity
sources = [local_env['OBJ_DIR']+'/'+s for s in orig_sources]

# -- STATIC LIBRARY
cppdefines_static = local_env['CPPDEFINES']
cppdefines_static['REFU_STATIC_LIB'] = None
static = local_env.StaticLibrary(outName, sources,
                                 CPPDEFINES=cppdefines_static)
Depends(static, options_header)
local_env.Alias('static', static)

# -- SHARED LIBRARY
cppdefines_shared = local_env['CPPDEFINES']
cppdefines_shared['REFU_DYNAMIC_LIB'] = None
shared = local_env.SharedLibrary(outName, sources,
                                 CPPDEFINES=cppdefines_shared)
Depends(shared, options_header)
local_env.Alias('shared', shared)


# -- UNIT TESTS
unit_tests_files = [
    'test_main.c',
    'utilities_for_testing.c',
    'test_string_core.c',
    'test_string_conversion.c',
    'test_string_retrieval.c',
    'test_string_files.c',
    'test_string_manipulation.c',
    'test_string_traversal.c',

    'test_utils_unicode.c',
    'test_utils_array.c',

    'test_io_files.c',
    'test_io_textfile.c',

    'test_log.c'
]
unit_tests_files = ['test/' + s for s in unit_tests_files]
unit_tests_files.extend(['src/' + s for s in orig_sources])
env.Check(unit_tests_files)
