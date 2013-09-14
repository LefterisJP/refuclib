from utils import build_msg

# These are variables from variables.py which contain a value
# and will be added to the build as preprocessor defines
vars_for_compile_time = [
    'FGETS_READ_BYTESN',
    'STRINGX_CAPACITY_MULTIPLIER',
    'DYNAMICARRAY_CAPACITY_MULTIPLIER',
    'LOCALSTACK_MEMORY_SIZE',
    'THREADX_MSGQUEUE_SIZE',
    'HASHMAP_LOAD_FACTOR',
    'ERROR_LOGGING_LEVEL',
    'WARNING_LOGGING_LEVEL',
    'INFO_LOGGING_LEVEL',
]

# These are variables from variables.py which are True/False
# and will be added to the build as preprocessor defines (or not)
truevars_for_compile_time = [
    'DEFAULT_ARGUMENTS',
    'SAFE_MEMORY_ALLOCATION',
    'DEBUG',
    'TEXTFILE_ADD_BOM'
]


def setupConfigVars(varEnv, env):
    """ Sets the configuration variables taken from the configuration file
        as defines to the construction environment

        varEnv     -- The temporary environment holding the variables read from
                   the configuration file
        env        -- The construction environment for which to setup the
                   configuration variables
    """
    # add to the environment all the compile time vars that are just defs
    for v in truevars_for_compile_time:
        if varEnv[v]:
            var_name = "RF_OPTION_{}".format(v)
            build_msg("Adding \"{}\" to the build environment".format(
                var_name), 'Info', env)
            env.Append(CPPDEFINES={var_name: None})
    # add to the environment all the compile time vars with a value
    for v in vars_for_compile_time:
        var_name = "RF_OPTION_{}".format(v)
        build_msg("Adding \"{}={}\" to the build environment".format(
            var_name, varEnv[v]), 'Info', env)
        env.Append(CPPDEFINES={'RF_OPTION_{}'.format(v):
                               varEnv[v]})
