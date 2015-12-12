from build_extra.config import set_debug_mode
from build_extra.utils import build_msg
import os

Import('env')

local_env = env.Clone()

# for now, library compiles always in debug mode.
# TODO: Figure out a better way
set_debug_mode(local_env, True)

orig_sources = [
    'Data_Structures/binaryarray.c',
    'Data_Structures/sbuffer.c',
    'Data_Structures/objset.c',
    'Data_Structures/intrusive_list.c',
    'Data_Structures/htable.c',
    'Data_Structures/mbuffer.c',
    'Data_Structures/strmap.c',
    'Utils/fixed_memory_pool.c',
    'Utils/endianess.c',
    'Utils/rf_unicode.c',
    'Utils/hash.c',
    'Utils/log.c',
    'Utils/array.c',
    'math/math.c',
    'math/ilog.c',
    'String/rf_str_commonp.c',
    'String/rf_str_conversion.c',
    'String/rf_str_core.c',
    'String/rf_str_filesx.c',
    'String/rf_str_traversalx.c',
    'String/rf_str_corex.c',
    'String/rf_str_files.c',
    'String/rf_str_retrieval.c',
    'String/rf_str_common.c',
    'String/rf_str_module.c',
    'String/rf_str_manipulationx.c',
    'String/rf_str_manipulation.c',
    'stdlib/io.c',
    'Parallel/rf_worker_pool_linux.c',
    'Parallel/rf_threading_linux.c',
    'Parallel/rf_threading.c',
    'refu.c',
    'Persistent/buffers.c',
    'Numeric/Integer/conversion.c',
    'IO/rf_file.c',
    'IO/rf_textfile.c',
]

if local_env['TARGET_SYSTEM'] == 'Linux':
    orig_sources += [
        'Time/time_linux.c',
        'System/rf_system_info_linux.c',
        'System/rf_system_linux.c',
        'System/rf_dlib_linux.c',
    ]
elif local_env['TARGET_SYSTEM'] == 'Windows':
    orig_sources += [
        'Time/time_win32.c',
        'System/rf_system_win32.c',
        'System/rf_system_info_win32.c',
        'System/rf_dlib_win32.c',
    ]
else:
    build_msg("Unsupported OS detected during source selection", "Error")
    Exit(1)

# only if actually building create the options file
options_header = SConscript(
    '#build_system/build_extra/clib/options_header_creator.py',
    exports='local_env')

# add specific environment variables
local_env.Append(CPPDEFINES={
    'FILE_BUFF_INITIAL_SIZE': local_env['INPUT_FILE_BUFF_INITIAL_SIZE']
})
local_env.Append(LIBS=['dl'])

# if we got PCRE2 add regular expressions
if local_env['has_pcre2']:
    local_env.ParseConfig("pcre2-config --libs8 --cflags")
    orig_sources += ['String/regex.c']


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
    'test_datastructs_strmap.c',
    'test_datastructs_darray.c',
    'test_datastructs_htable.c',

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
