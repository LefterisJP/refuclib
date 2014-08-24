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

# set the obj dir and prepend obj dir to all sources
local_env.VariantDir(local_env['OBJ_DIR'], "src", duplicate=0)
# a list comprehension prepending the obj dir to all of the sources
# (instead of the sourcedir) ... Scons peculiarity
sources = [local_env['OBJ_DIR']+'/'+s for s in orig_sources]

# -- STATIC LIBRARY
cppdefines_static = local_env['CPPDEFINES']
cppdefines_static['REFU_STATIC_LIB'] = None
clib_static = local_env.StaticLibrary(local_env['CLIB_OUT_NAME'],
                                      sources,
                                      CPPDEFINES=cppdefines_static)
Depends(clib_static, options_header)
local_env.Alias('clib_static', clib_static)

# -- SHARED LIBRARY
cppdefines_shared = local_env['CPPDEFINES']
cppdefines_shared['REFU_DYNAMIC_LIB'] = None
clib_shared = local_env.SharedLibrary(local_env['CLIB_OUT_NAME'],
                                      sources,
                                      CPPDEFINES=cppdefines_shared)
Depends(clib_shared, options_header)
local_env.Alias('clib_shared', clib_shared)


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
clib_tests = local_env.Check(
    target="clib_tests",
    source=unit_tests_files)
local_env.Alias('clib_tests', clib_tests)
