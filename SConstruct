from build_extra.config import set_debug_mode
import os

Import('env')

local_env = env.Clone()

# for now, library compiles always in debug mode.
# TODO: Figure out a better way
set_debug_mode(local_env, True)

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
    'test_string_buffers.c',

    'test_utils_unicode.c',
    'test_utils_array.c',
    'test_utils_memory_pools.c',
    'test_datastructs_objset.c',
    'test_datastructs_mbuffer.c',
    'test_datastructs_sbuffer.c',

    'test_intrusive_list.c',

    'test_io_files.c',
    'test_io_textfile.c',

    'test_log.c'
]

test_env = local_env.Clone()
test_env.Append(CHECK_EXTRA_DEFINES={
    'CLIB_TESTS_PATH':
    "\\\"" + os.path.abspath(
        os.path.join(test_env['CLIB_DIR'], 'test')
    ) + "/\\\""
})
test_env.Append(CHECK_SINGLE_FILE_OPTIONS={
    # This file tests invalid UTF8 string literals and will
    # generate this warning in clang
    'test_utils_unicode.c': ['-Wno-invalid-source-encoding'],
})
test_sources = [os.path.join("test", s) for s in unit_test_files]
test_sources.extend([os.path.join("src", s) for s in orig_sources])
clib_tests = test_env.Check(
    target="clib_tests",
    source=test_sources)
local_env.Alias('clib_tests', clib_tests)

# Return the built static library so that the compiler can link against it
Return('clib_static')
