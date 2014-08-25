import os
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

# -- STATIC LIBRARY
static_env = local_env.Clone()
static_env.Append(CPPDEFINES='REFU_STATIC_LIB')
static_env.VariantDir("build_static", "src", duplicate=0)
static_sources = [os.path.join("build_static", s) for s in orig_sources]
clib_static = static_env.StaticLibrary(
    local_env['CLIB_OUT_NAME'],
    # source=[static_env.Object(s) for s in static_sources])
    source=static_sources)
Depends(clib_static, options_header)
local_env.Alias('clib_static', clib_static)

# -- SHARED LIBRARY
shared_env = local_env.Clone()
shared_env.Append(CPPDEFINES='REFU_DYNAMIC_LIB')
shared_env.VariantDir("build_shared", "src", duplicate=0)
shared_sources = [os.path.join("build_shared", s) for s in orig_sources]
clib_shared = shared_env.SharedLibrary(
    local_env['CLIB_OUT_NAME'],
    source=shared_sources)
Depends(clib_shared, options_header)
local_env.Alias('clib_shared', clib_shared)


# -- UNIT TESTS
unit_test_files = [
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

test_env = local_env.Clone()
test_env.VariantDir("build_test/test", "test", duplicate=1)
test_env.VariantDir("build_test/src", "src", duplicate=1)
test_sources = [os.path.join("build_test/test", s) for s in unit_test_files]
test_sources.extend([os.path.join("build_test/src", s) for s in orig_sources])
clib_tests = test_env.Check(
    target="clib_tests",
    source=test_sources)
local_env.Alias('clib_tests', clib_tests)

# Return the built static library so that the compiler can link against it
Return('clib_static')
