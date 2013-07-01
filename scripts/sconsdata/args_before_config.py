import ast
import os

def extraobjects_to_list(s):
    """
       A function to convert the extra objects string to a list
    """
    # print("--\n{}\n--\n".format(s))
    if s == "":
        return []
    try:
        return ast.literal_eval(s)
    except Exception as err:
        print type(err)
        print(err)
        print("**ERROR** Provided an extra objects string but it could "
              "not be properly evaluated as a python literal")
        Exit(-1)

args_before = Variables()
args_before.Add(PathVariable(
    'CONFIG_FILE',
    'This is the path to the configuration file where all the variables'
    'will be read from.',
    '../../refu.config',
    PathVariable.PathIsFile))

args_before.Add(
    PathVariable('REFU_DIR', 'The root directory of the refu library.'
                 ' Absolute value', '../..',PathVariable.PathIsDir))

args_before.Add(
    'EXTRA_OBJECTS',
    'With this option you can provide extra object types to the template system'
    ' so that they can be added as possible specialization options for data '
    'structures and other modules that may use them. The syntax to achieve '
    'this is a bit challenging. You have to provide it as a string that will'
    ' be interpreted as a python literal. This string must contain a list of '
    'lists. Note the usage of \"\".\n'
    'Each member of that list must be in the form of:\n'
    '[\"name\", \"source_name\", \"source\", \"header\",'
    ' [\"destroy_function\", \"copy_function\", \"compare_function\",'
    '  ["header1", \"header2\"] ] ]'
    '\n\nFollowing is an explanation of the lists members:\n'
    '\tname -- The name of the object. Will be appended to all of the functions\n'
    '\tsource_name -- The name of the object as defined in the source code\n'
    '\tsource -- the path to the source file which implements the code of the functions of this object\n'
    '\theader -- the path to the header file that defines the functions and '
    'the struct of this object\n'
    '\tAnother list to act as the key of the obj_dict. This will contain\n'
    '\t\t- destroy function for the object\n'
    '\t\t- copy function for the object\n'
    '\t\t- compare function for the object\n'
    '\t\t- A list of headers needed to compile the source file\n',
    '',
    converter=extraobjects_to_list)

#return the variables, and make sure the paths are absolute
temp = Environment(variables = args_before)
config_file = os.path.abspath(temp['CONFIG_FILE'])
extra_objects = temp['EXTRA_OBJECTS']
refu_dir = os.path.abspath(temp['REFU_DIR'])

Return('config_file extra_objects refu_dir args_before')
