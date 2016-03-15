from build_extra.config import set_debug_mode
from build_extra.utils import build_msg
import os

Import('env')

local_env = env.Clone()

# for now, library compiles always in debug mode.
# TODO: Figure out a better way
set_debug_mode(local_env, True)

unit_test_files = []
orig_sources = [
    'datastructs/binaryarray.c',
    'datastructs/sbuffer.c',
    'datastructs/objset.c',
    'datastructs/intrusive_list.c',
    'datastructs/htable.c',
    'datastructs/mbuffer.c',
    'datastructs/strmap.c',
    'utils/fixed_memory_pool.c',
    'utils/endianess.c',
    'utils/rf_unicode.c',
    'utils/hash.c',
    'utils/log.c',
    'utils/array.c',
    'math/math.c',
    'math/ilog.c',
    'string/commonp.c',
    'string/conversion.c',
    'string/core.c',
    'string/filesx.c',
    'string/traversalx.c',
    'string/corex.c',
    'string/files.c',
    'string/retrieval.c',
    'string/common.c',
    'string/module.c',
    'string/manipulationx.c',
    'string/manipulation.c',
    'stdlib/io.c',
    'parallel/rf_worker_pool_linux.c',
    'parallel/rf_threading_linux.c',
    'parallel/rf_threading.c',
    'refu.c',
    'persistent/buffers.c',
    'numeric/Integer/conversion.c',
    'io/rf_file.c',
    'io/rf_textfile.c',
]

if local_env['TARGET_SYSTEM'] == 'Linux':
    orig_sources += [
        'time/time_linux.c',
        'system/rf_system_info_linux.c',
        'system/rf_system_linux.c',
        'system/rf_dlib_linux.c',
    ]
elif local_env['TARGET_SYSTEM'] == 'Windows':
    orig_sources += [
        'time/time_win32.c',
        'system/rf_system_win32.c',
        'system/rf_system_info_win32.c',
        'system/rf_dlib_win32.c',
    ]
else:
    build_msg("Unsupported OS detected during source selection", "Error")
    Exit(1)

# only if actually building create the options file
options_header = SConscript(
    '#build_system/build_extra/clib/options_header_creator.py',
    exports='local_env'
)

# add specific environment variables
local_env.Append(CPPDEFINES={
    'FILE_BUFF_INITIAL_SIZE': local_env['INPUT_FILE_BUFF_INITIAL_SIZE']
})
local_env.Append(LIBS=['dl'])

# if we got PCRE2 add regular expressions
if local_env['has_pcre2']:
    local_env.ParseConfig("pcre2-config --libs8 --cflags")
    orig_sources += ['string/regex.c']
    unit_test_files += ['test_regex.c']


# -- STATIC LIBRARY
static_env = local_env.Clone()
static_env.Append(CPPDEFINES='REFU_STATIC_LIB')
static_env.VariantDir("build_static", "src", duplicate=0)
static_sources = [os.path.join("build_static", s) for s in orig_sources]
clib_static = static_env.StaticLibrary(
    local_env['CLIB_OUT_NAME'],
    source=static_sources
)
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
unit_test_files += [
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
