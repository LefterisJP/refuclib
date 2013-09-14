import ast
import os

from utils import build_msg

store_type_values = ['all', 'shallow', 'deep']
extra_dict_required_keys = ['name', 'source_name', 'headers']

def validate_extra_objects(extra_dict):
    """
    Checks that the EXTRA_OBJECTS list of dictionaries of objects
    provided contains sane data
    """
    for obj in extra_dict:
        if not all(key in obj for key in extra_dict_required_keys):
            build_msg("The dictionary of an extra object must contain at least"
                      " the following keys: {}".format(', '.join(
                          extra_dict_required_keys)
            ))
            Exit(-1)
        if ("store_type" in obj.keys() and
            obj["store_type"] not in store_type_values):
            build_msg("Illegal value given for extra object \"store_type\" key."
                      "Legal values are: [{}]".format(', '.join(
                          store_type_values
            )))
            Exit(-1)

    return extra_dict

def extraobjects_to_dict(s):
    """
       A function to convert the extra objects string to a dict
    """
    # print("--\n{}\n--\n".format(s))
    if s == "":
        return {}
    # that's a pretty stupid way to make the dict/list be all strings but
    # it should work
    s = s.replace(":", "\":\"")
    s = s.replace("{", "{\"")
    s = s.replace("}", "\"}")
    s = s.replace(",", "\",\"")
    s = s.replace(":\"[", ":[\"")
    s = s.replace("]\"", "\"]")
    s = s.replace("\"}\",\"{\"", "\"},{\"")
    try:
        d = ast.literal_eval(s)
        return validate_extra_objects(d)
    except Exception as err:
        print type(err)
        print(err)
        build_msg("The EXTRA_OBJECTS string provided could not be properly"
                  " evaluated as a python literal", "Error")
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
                 ' Absolute value', '../..', PathVariable.PathIsDir))

args_before.Add(
    'EXTRA_OBJECTS',
    'With this option you can provide extra object types to the template system'
    ' so that they can be added as possible specialization options for data '
    'structures and other modules that may use them. The syntax to achieve '
    'this is the following:'
    ' EXTRA_OBJECTS=\"[{name:test_object,'
    'source_name:test_obj,'
    'headers:[Tests/ExtraObjects/test.h,<include/something.h>, ...],'
    'destroy_func:test_destroy,'
    'copy_func:test_copy,'
    'compare_func:test_equal'
    'store_type:both'
    '}, ..., ... ]'
    'Simply replace test with the object in question. Notice there is no need'
    'to encircle keys or values of the dict with \"\". They are added by the '
    'build script. You only need to enclose the whole dictionary once in \"\"'
    'From the above keys only up to the header name are required',
    '',
    converter=extraobjects_to_dict)

#return the variables, and make sure the paths are absolute
temp = Environment(variables=args_before)
config_file = os.path.abspath(temp['CONFIG_FILE'])
extra_objects = temp['EXTRA_OBJECTS']
refu_dir = os.path.abspath(temp['REFU_DIR'])

Return('config_file extra_objects refu_dir args_before')
