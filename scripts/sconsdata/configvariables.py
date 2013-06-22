""" Sets the configuration variables taken from the configuration file as defines to the construction environment

	varEnv 	-- The temporary environment holding the variables read from the configuration file
	env		-- The construction environment for which to setup the configuration variables
"""
def setupConfigVars(varEnv,env):
	if varEnv['DEFAULT_ARGUMENTS'] == True:
		env.Append(CPPDEFINES	= {'RF_OPTION_DEFAULT_ARGUMENTS':None})
	if varEnv['SAFE_MEMORY_ALLOC'] == True:
		env.Append(CPPDEFINES	= {'RF_OPTION_SAFE_MEMORY_ALLOCATION':None})
	if varEnv['VERBOSE_ERROR_LOGGING'] == True:
		env.Append(CPPDEFINES	= {'RF_OPTION_VERBOSE_ERRORS':None})
	if varEnv['TEXTFILE_ADD_BOM'] == True:
		env.Append(CPPDEFINES	= {'RF_OPTION_FILE_ADDBOM': None})
		
	env.Append(CPPDEFINES	=
               {'RF_OPTION_FGETS_READBYTESN':varEnv['FGETS_READ_BYTESN']})
	env.Append(CPPDEFINES	=
               {'RF_OPTION_STRINGX_CAPACITY_M':
                varEnv['STRINGX_CAPACITY_MULTIPLIER']})
	env.Append(CPPDEFINES	= 
               {'RF_OPTION_DYNAMICARRAY_CAPACITY_M':
                varEnv['DYNAMICARRAY_CAPACITY_MULTIPLIER']})
	env.Append(CPPDEFINES	=
               {'RF_OPTION_LOCALSTACKMEMORY_SIZE':
                varEnv['LOCAL_STACK_SIZE']})
	env.Append(CPPDEFINES	=
               {'RF_OPTION_THREADX_MSGQUEUE':
                varEnv['THREADX_MSGQUEUE_SIZE']})	
