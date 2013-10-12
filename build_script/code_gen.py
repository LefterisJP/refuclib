import shutil
import os
import json
import sys
from time import gmtime, strftime

def enum(*sequential, **named):
    enums = dict(zip(sequential, range(len(sequential))), **named)
    return type('Enum', (), enums)

json_file_name = "temp_build_data.json"


def gen_template_intro_str(module_name, type_name):
    return (
        "/**\n"
        " * This is a template generated file by the build system at\n"
        " * {}\n"
        " * for module \"{}\" and for data type \"{}\"\n"
        " */\n\n\n"
    ).format(strftime("%Y-%m-%d %H:%M:%S", gmtime()), module_name, type_name)

class MutateTargetError(Exception):
    def __init__(self, target_type, file_name, line):
        self.target_type = target_type
        self.file_name = file_name
        self.line = line
    def __str__(self):
        return ("Unrecognized mutation target type \"{}\" found in the "
        "source of {} at line:\"{}\"".format(
            self.target_type, self.file_name, self.line)
        )

class MutateSourceError(Exception):
    def __init__(self, source_type, file_name, line):
        self.source_type = source_type
        self.file_name = file_name
        self.line = line
    def __str__(self):
        return ("Unrecognized mutation source type \"{}\" found in the "
        "source of {} at line:\"{}\"".format(
            self.source_type, self.file_name, self.line)
        )

class MutateError(Exception):
    def __init__(self, msg, file_name, line):
        self.file_name = file_name
        self.msg = msg
        self.line = line
    def __str__(self):
        return ("Mutation Error: \"{}\" "
                "in {} at line:\"{}\"".format(
                    self.msg, self.file_name, self.line)
        )

class ParsingError(Exception):
    def __init__(self, msg, file_name, line):
        self.file_name = file_name
        self.msg = msg
        self.line = line
    def __str__(self):
        return ("Parsing Error: \"{}\" "
                "in {} at line:\"{}\"".format(
                    self.msg, self.file_name, self.line)
        )

class OmitConditionError(Exception):
    def __init__(self, condition, file_name, line):
        self.condition = condition
        self.file_name = file_name
        self.line = line
    def __str__(self):
        return ("Unrecognized omit condition \"{}\" found in the"
        "source of {} at line:\"{}\"".format(
            self.condition, self.file_name, self.line)
        )

class OmitInbalanceError(Exception):
    def __init__(self, ondition, file_name, line):
        self.file_name = file_name
        self.line = line
    def __str__(self):
        return ("Inbalanced omit found in the"
        "source of {} at line:\"{}\"".format(
            self.file_name, self.line)
        )

class ExtraObjectError(Exception):
    def __init__(self, name, msg):
        self.name = name
        self.msg = msg
    def __str__(self):
        return "Extra Object \"{}\" Related Error:\n{}".format(self.name,
                                                               self.msg)

omit_start = "/* @omit start */"
omit_cond = "/* @omitcond "
omit_end = "/* @omit end */"
omit_next = "/* @omit next */"
mutate_cmd = "/* @mutate"

addinc_start = "/* @additional_includes start */"
addinc_end = "/* @additional_includes end */"

def get_func1_args(line, func_name):
    """
       Returns the argument of an 1 argument function
    """
    part = line.partition(func_name+"(")
    #count how many open parentheses we had before
    openP = part[0].count("(")
    line = part[2]
    #take only the relevant parenetheses part
    arg = line
    for i in range(openP):
        arg = arg.rpartition(")")[0]
    arg = arg.rpartition(")")[0]
    arg = arg.strip()
    return arg

def get_func2_args(line, func_name):
    """
       Returns a tuple with the arguments of a 2 arguments function
    """
    part = line.partition(func_name+"(")
    #count how many open parentheses we had before
    openP = part[0].count("(")
    line = part[2]
    part = line.partition(",")
    arg1 = part[0].strip()
    #take only the relevant parenetheses part
    arg2 = part[2]
    for i in range(openP):
        arg2 = arg2.rpartition(")")[0]
    arg2 = arg2.rpartition(")")[0]
    arg2 = arg2.strip()
    return (arg1, arg2)

def get_func3_args(line, func_name):
    """
       Returns a tuple with the arguments of a 3 arguments function
    """
    part = line.partition(func_name+"(")
    #count how many open parentheses we had before
    openP = part[0].count("(")
    line = part[2]
    part = line.partition(",")
    arg1 = part[0].strip()
    #take only the relevant parenetheses part
    arg2 = part[2]
    part = arg2.partition(",")
    arg2 = part[0].strip()
    arg3 = part[2]
    for i in range(openP):
        arg3 = arg3.rpartition(")")[0]
    arg3 = arg3.rpartition(")")[0]
    arg3 = arg3.strip()
    return (arg1, arg2, arg3)


def check_extra_args(line, mutate, type_d):
    """
       Checks for extra mutate arguments and applies some transformations
       on the line right before the main handler
    """
    #no extra args
    if len(mutate) < 3:
        return line

    #silly method to get rid of element size
    if mutate[2] == "NOSIZE":
        line = line.replace("*dst->elementSize", "")
        line = line.replace("*src->elementSize", "")
        line = line.replace("*l->elementSize", "")
        line = line.replace("dst->elementSize", "")
        line = line.replace("src->elementSize", "")
        line = line.replace("l->elementSize", "")
        return line

    #in any other case just return line as is
    return line



class CodeGen():
    """
       This class encapsulates all of the code generating functionality
    """

    refu_objects = [ "String" ]
    lms_list = ["String"]

    def __init__(self, refu_root):
        """
           Initializes the codegen object, either creating the object and
           types dicts and list or loading them from a file
        """
        self.handle_dict = {
            "TYPE": self.handle_TYPE,
            "TYPEPTR": self.handle_TYPEPTR,
            "TYPEPTR_OBJ_ONLY": self.handle_TYPEPTR_OBJ_ONLY,
            "SIZE": self.handle_SIZE,
            "REFERENCE": self.handle_REFERENCE,
            "ASSIGN": self.handle_ASSIGN,
            "COMPARE": self.handle_COMPARE,
            "INIT": self.handle_INIT,
            "DESTROY": self.handle_DESTROY,
            "INITIALIZE_TO_ZERO": self.handle_INITIALIZE_TO_ZERO,
            "REMOVE": self.handle_REMOVE,
            "REPLACE": self.handle_REPLACE,
            "MEMCPY": self.handle_MEMCPY
        }

        # types that can be safely templated from within the library
        self.library_types = { "I": "int",
                          "UI": "unsigned int",
                          "I8": "int8_t",
                          "UI8": "uint8_t",
                          "I16": "int16_t",
                          "UI16": "uint16_t",
                          "I32": "int32_t",
                          "UI32": "uint32_t",
                          "I164": "int64_t",
                          "UI64": "uint64_t",
                          "String": "RF_String",
                          "generic": "generic",
            }

        if os.path.isfile(os.path.join(refu_root, json_file_name)):
            self.load_data_from_json(json_file_name)
        else:
            self.type_dict = { "I": "int",
                          "UI": "unsigned int",
                          "I8": "int8_t",
                          "UI8": "uint8_t",
                          "I16": "int16_t",
                          "UI16": "uint16_t",
                          "I32": "int32_t",
                          "UI32": "uint32_t",
                          "I164": "int64_t",
                          "UI64": "uint64_t",
                          "String": "RF_String",
                          "generic": "generic",
            }

            self.obj_dict = {
                "String": {
                    "destroy_func": "rfString_Deinit",
                    "copy_func": "rfString_Copy_IN",
                    "compare_func": "rfString_Equal",
                    "headers": ["Definitions/imex.h", # for import export macro
                                "Definitions/defarg.h", # for default arguments
                                "Preprocessor/rf_tokens.h", #for defined library tokens
                                "Definitions/types.h",  #for fixed size data types
                                "String/string_decl.h",  #for RF_String
                                "String/core.h"]  #for Equal, Copy_IN and Deinit
                }
            }
        self.omitting = False
        self.omit_counter = 0
        self.omitting_next = False


    def save_data_to_json(self, filename):
        outF = open(filename, 'w')
        temp_list = [self.type_dict, self.obj_dict]
        outF.write(json.dumps(temp_list))
        outF.close()

    def load_data_from_json(self, filename):
        inF = open(filename)
        data = json.load(inF)
        self.type_dict = data[0]
        self.obj_dict = data[1]
        inF.close()

    def check_line_for_name_sub(self, line, subs, type_d,
                                source_suffix, header_suffix):
        """
        Replaces the name subs from the template for this line.

        If the template is of specialized type then a suffix is added. For example
        a 'd' for deep and an 's' for shallow.
        But if the line is an include the equivalent full header suffix like 'deep'
        or 'shallow' is added. Shallow copies of POD should also not get a source
        suffix since POD can only be shallow copied.
        """
        if header_suffix == "shallow" and type_d not in self.obj_dict:
            reps = "_" + type_d
            rephs = "_" + header_suffix + "_" + type_d
        elif source_suffix != "":
            reps = source_suffix + "_" + type_d
            rephs = "_" + header_suffix + "_" + type_d
        else:
            reps = "_" + type_d
            rephs = "_" + type_d
        for s in subs:
            if s in line and "#include" in line:
                line = line.replace(s, s + rephs)
            elif s in line:
                line = line.replace(s, s + reps)
        return line


    def add_object(self, extra_dict, refu_dir):
        """
        Adds an object to the code generation.
        Look at args_before_config.py for the details
        """
        self.type_dict[extra_dict["name"]] = extra_dict["source_name"]
        self.obj_dict[extra_dict["name"]] = {
            "destroy_func": extra_dict.get("destroy_func"),
            "copy_func": extra_dict.get("copy_func"),
            "compare_func": extra_dict.get("compare_func"),
            "headers": extra_dict.get("headers"),
            "store_type": extra_dict.get("store_type")
        }
        #since a new object has been added save it to the json file
        fname = os.path.join(refu_dir, json_file_name)
        if os.path.isfile(fname):
           os.remove(fname)
        self.save_data_to_json(fname)


    def code_gen(self, dstruct_types, root, type_d, name_subs):
        """
            This is the main function that creates the 3 source files
            for the named template according to the specified parameters.
            If any errors happen they are reported by this function and then
            the exception is re-raised

            The function returns a list of the generated source files
        """
        generated_sources = []
        try:
            # for each type of data structure in template files create the files
            for dstruct in dstruct_types:

                source_suffix = dstruct['source_suffix']
                if type_d not in self.obj_dict:
                    #POD don't have a deep data structure
                    if dstruct['type'] == "deep": 
                        continue
                    #POD always go with shallow so no extra suffix needed
                    source_suffix = ""

                if dstruct['type'] != "":
                    oldpre = dstruct['name'] + "_" + dstruct['type']
                    newpre = dstruct['name'] + "_" + dstruct['type'] + "_" + type_d
                else:
                    oldpre = dstruct['name']
                    newpre = dstruct['name'] + "_" + type_d
                #generate _decl.h
                self.gen_single_source(
                    os.path.join(
                        root, "include",
                        dstruct['parent_dir'],
                        oldpre + "_decl.htemplate"),
                    os.path.join(
                        root, "include",
                        dstruct['parent_dir'],
                        newpre + "_decl.h"),
                    type_d,
                    name_subs,
                    source_suffix,
                    dstruct['type']
                )

                #generate .h
                self.gen_single_source(
                    os.path.join(
                        root, "include",
                        dstruct['parent_dir'],
                        oldpre + ".htemplate"),
                    os.path.join(
                        root, "include",
                        dstruct['parent_dir'],
                        newpre + ".h"),
                    type_d,
                    name_subs,
                    source_suffix,
                    dstruct['type']
                )
                #generate .c
                self.gen_single_source(
                    os.path.join(
                        root, "src",
                        dstruct['parent_dir'],
                        oldpre + ".ctemplate"),
                    os.path.join(
                        root, "src",
                        dstruct['parent_dir'],
                        newpre + ".c"),
                    type_d,
                    name_subs,
                    source_suffix,
                    dstruct['type']
                )

                #remember the source from parent dir and down
                generated_sources.append(os.path.join(dstruct['parent_dir'],
                                                      newpre + ".c"
                ))


        except (MutateError, MutateSourceError, MutateTargetError,
                OmitConditionError, OmitInbalanceError, ParsingError) as err:
            print type(err)
            print(err)
            raise err
        except Exception as err:
            print("Unexpected Problems during code generation")
            exc_type, exc_obj, exc_tb = sys.exc_info()
            fname = os.path.split(exc_tb.tb_frame.f_code.co_filename)[1]
            print("\tError type:{} \n\tFile Name:{}"
                  "\n\tError: {}\n\tLine number: {}".format(
                  exc_type, fname, str(err), exc_tb.tb_lineno))
            raise err

        return generated_sources





    def handle_COMPARE(self, line, mutate, type_d, file_name, line_num):
        """
           Performs the mutation of the generic template code for
           comparing objects. It either gets changed into:
           - If simple data types then plain old data equals comparison
           - If an object, then that objects's equals/comparison function
             with the arguments address ('r'eference or 'p'ointer) being
             decided by the structure of mutate[0] (mutate_from)
        """
        if "ptr2Equal" in mutate[0]:
            (arg1, arg2) = get_func2_args(line, "ptr2Equal")
            if type_d in self.obj_dict:
                if self.obj_dict[type_d]["compare_func"] is None:
                    raise MutateError( "Source template requires compare "
                                       "function while none was given at "
                                       "definition",
                                       file_name, line_num)

                if mutate[0] == "ptr2Equalpr":
                    return "if({}({}, &{}))\n".format(
                        self.obj_dict[type_d]["compare_func"],
                        arg1,
                        arg2)
                elif mutate[0] == "ptr2Equalpp":
                    return "if({}({}, {}))\n".format(
                        self.obj_dict[type_d]["compare_func"],
                        arg1,
                        arg2)
                elif mutate[0] == "ptr2Equalrr":
                    return "if({}(&{}, &{}))\n".format(
                        self.obj_dict[type_d]["compare_func"],
                        arg1,
                        arg2)
                elif mutate[0] == "ptr2Equalrp":
                    return "if({}(&{}, {}))\n".format(
                        self.obj_dict[type_d]["compare_func"],
                        arg1,
                        arg2)
                else:
                    raise MutateSourceError(mutate[0], file_name, line_num)
            else:
                return "if({} == {})\n".format(arg1, arg2)
        else:
            raise MutateSourceError(mutate[0], file_name, line_num)



    def handle_assign_function(self, line, mutate,
                               type_d, file_name, func_name,
                               line_num):
        (arg1, arg2) = get_func2_args(line, func_name)
        if type_d in self.obj_dict:
            if self.obj_dict[type_d]["copy_func"] is None:
                raise MutateError( "Source template requires copy "
                                   "function while none was given at "
                                   "definition",
                                   file_name, line_num)
            # read the IFBLOCK and get the rest of the arguments for
            # as statements inside if
            if "IFBLOCK" not in mutate:
                raise MutateError("For an assignment to objects, an "
                                  "IFBLOCK argument has not been given",
                                  file_name, line_num)
            ifblock = ""
            for i in range(mutate.index("IFBLOCK") + 1, len(mutate)):
                ifblock = ifblock + " " + mutate[i];
            if mutate[0] == "{}rr".format(func_name):
                return "if(!{}(&{}, &{}))\n\t{{ {} }}\n".format(
                    self.obj_dict[type_d]["copy_func"],
                    arg1,
                    arg2,
                    ifblock)
            elif mutate[0] == "{}pp".format(func_name):
                return "if(!{}({},{}))\n\t{{ {} }}\n".format(
                    self.obj_dict[type_d]["copy_func"],
                    arg1,
                    arg2,
                    ifblock)
            elif mutate[0] == "{}pr".format(func_name):
                return "if(!{}({},&{}))\n\t{{ {} }}\n".format(
                    self.obj_dict[type_d]["copy_func"],
                    arg1,
                    arg2,
                    ifblock)
            elif mutate[0] == "{}rp".format(func_name):
                return "if(!{}(&{},{}))\n\t{{ {} }}\n".format(
                    self.obj_dict[type_d]["copy_func"],
                    arg1,
                    arg2,
                    ifblock)
            else:
                raise MutateSourceError(mutate[0], file_name, line_num)
        else:
            if "PODPTR" in mutate:
                if "POD_DLR" in mutate: # dereference POD left and right
                    return "*({}) = *({});\n".format(arg1, arg2);
                elif "POD_DL" in mutate: # derefence POD left
                    return "*({}) = {};\n".format(arg1, arg2);
                elif "POD_DR" in mutate: # dereference POD right
                    return "{} = *({});\n".format(arg1, arg2);
                elif "POD_NDR" in mutate: # do not dereference POD at all
                    return "{} = {};\n".format(arg1, arg2);
                else:
                    raise MutateError("Unknown ptr2Copy handler for POD "
                                      "data has been given", file_name,
                                      line_num)
            else:
                return "{} = {};\n".format(arg1, arg2)

    def handle_ASSIGN(self, line, mutate, type_d, file_name, line_num):
        """
           Performs the mutation of the generic template code for
           copying objects. It either gets changed into:
           - If simple data types then plain old data assignment
           - If an object, then that objects's copy function
        """
        if("ptr2Copy" in mutate[0]):
            return self.handle_assign_function(line, mutate, type_d,
                                               file_name, "ptr2Copy",
                                               line_num)
        else:
            raise MutateSourceError(mutate[0], file_name, line_num)

    def handle_MEMCPY(self, line, mutate, type_d, file_name, line_num):
        """
           Performs the mutation of the generic template code for
           shallow copying objects.
        """
        #kind of a hack but when mutating ignore all '&' operators
        line = line.replace("&", "")
        (arg1, arg2, arg3) = get_func3_args(line, "memcpy")
        if type_d in self.obj_dict:
            return "{} = *{};\n".format(arg1, arg2)
        else:
            return "{} = {};\n".format(arg1, arg2)

    def handle_DESTROY(self, line, mutate, type_d, file_name, line_num):
        """
           Performs the mutation of the generic template code for
           destroying objects. It either gets changed into:
           - If simple data types then omit
           - If an object, then that objects's destroy function
        """
        if("ptr2Destroy" in mutate[0]):
            arg = get_func1_args(line, "ptr2Destroy")
            if type_d in self.obj_dict:
                if self.obj_dict[type_d]["destroy_func"] is None:
                    raise MutateError( "Source template requires destroy "
                                       "function while none was given at "
                                       "definition",
                                       file_name, line_num)
                if mutate[0] == "ptr2Destroyr":
                    return "{}(&{});\n".format(
                        self.obj_dict[type_d]["destroy_func"],
                        arg)
                elif mutate[0] == "ptr2Destroyp":
                    return "{}({});\n".format(
                        self.obj_dict[type_d]["destroy_func"],
                        arg)
                else:
                    raise MutateSourceError(mutate[0], file_name, line_num)
            else:
                return "";

    def handle_INITIALIZE_TO_ZERO(self, line, mutate, type_d, file_name, line_num):
        """
           If type is not plain old data, mutate[0] (mutate_from) is changed
           into a compound literal structure initialization to ZERO
        """
        if type_d in self.obj_dict:
            return line.replace(mutate[0],
                                "(const struct {}) {{0}}".format(
                self.type_dict[type_d]))
        else:
            return line

    def handle_INIT(self, line, mutate, type_d, file_name, line_num):
        """
            If type is not plain old data, mutate[0] (mutate_from) is changed
            into the initialization function of the data structure's object

            TODO: If actually needed anywhere
        """
        return line

    def handle_REFERENCE(self, line, mutate, type_d, file_name, line_num):
        """
           If type is not plain old data, mutate[0] (mutate_from) is changed
           into a reference
        """
        if type_d in self.obj_dict:
            return line.replace(mutate[0],"&"+mutate[0] )
        else:
            return line

    def handle_TYPE(self, line, mutate, type_d, file_name, line_num):
        """
           mutate[0] (mutate_from) is changed into the type
        """
        return line.replace(mutate[0], self.type_dict[type_d])

    def handle_SIZE(self, line, mutate, type_d, file_name, line_num):
        """
           mutate[0] (mutate_from) is changed into the size of the type
        """
        return line.replace(mutate[0], "sizeof({})".format(
            self.type_dict[type_d]))

    def handle_TYPEPTR_OBJ_ONLY(self, line, mutate, type_d, file_name, line_num):
        """
           mutate[0] (mutate_from) is changed into the type if the type is
           plain old data. If not then it's changed into type pointer
        """
        if type_d in self.obj_dict:
            return line.replace(mutate[0], self.type_dict[type_d]+"*")
        else:
            return line.replace(mutate[0], self.type_dict[type_d])

    def handle_TYPEPTR(self, line, mutate, type_d, file_name, line_num):
        """
           mutate[0] (mutate_from) is changed into type pointer
        """
        return line.replace(mutate[0], self.type_dict[type_d]+"*")

    def handle_REMOVE(self, line, mutate, type_d, file_name, line_num):
        """
            Handle the simple case of requesting removal of something
            If there is a specific condition in mutate check for it.
        """
        if "POD" in mutate and type_d in self.obj_dict:
            return line
        return line.replace(mutate[0], "")

    def handle_REPLACE(self, line, mutate, type_d, file_name, line_num):
        """
            Handle the simple case of requesting replacing something
            with something else
        """
        if len(mutate) != 3:
            raise MutateError("Incorrect arguments given for replacement",
                              file_name, line_num)

        return line.replace(mutate[0], mutate[2])

    def handle_mutate(self, mutate, type_d, line, name, line_num):
        if len(mutate) < 2:
            raise MutateError("mutate list with less than 2 arguments passed "
                              "to handler", name, line_num)
        #if the 2nd argument to mutate is unknown we got a problem
        if mutate[1] not in self.handle_dict:
            raise MutateTargetError(mutate[1], name, line_num)
        # handle any extra arguments first
        line = check_extra_args(line, mutate, type_d)
        # and then call the "main" handler
        return self.handle_dict[mutate[1]](line, mutate, type_d, name, line_num)

    def handle_omit_cond(self, line, type_d, file_name, line_num):
        """
           Handles omits with conditions. True means omitting
        """
        ret = False
        part = line.partition(omit_cond)
        if part[0] == line:
            raise ParsingError("Could not find beginning of @omit cond "
                               "statement", file_name, line_num)
        line = part[2]

        part = line.partition("*/")
        if part[0] == line:
            raise ParsingError("Comment did not end at the same line for "
                               "@omit cond statement", file_name, line_num)
        line = part[0]

        conditions = line.split()
        for cond in conditions:
            if "OBJECT" == cond:
                if type_d in self.obj_dict:
                    ret = True
            elif "POD" == cond:
                if type_d not in self.obj_dict:
                    ret = True
            elif "NONLMS" == cond:
                if type_d not in self.lms_list:
                    ret = True
            elif "STRING" == cond:
                if type_d == "String":
                    ret = True;
            elif "SHALLOW_COPY_ONLY" == cond:
                if(type_d in self.obj_dict and
                   self.obj_dict[type_d].get("store_type") == "shallow"):
                    ret = True
            elif "DEEP_COPY_ONLY" == cond:
                if(type_d in self.obj_dict and
                   self.obj_dict[type_d].get("store_type") == "deep"):
                    ret = True
            else:
                raise OmitConditionError(cond, file_name, line_num)

        return ret



    def parser_check_omits(self, line, type_d, name, line_num):
        """
        Checks off a line as far as @omit statements are concerned
        Returns either:
        True -- line should be skipped
        False -- keep parsing
        """

        if self.omitting_next:
            self.omitting_next = False
            return True

        if not self.omitting and omit_cond in line:
            self.omitting = self.handle_omit_cond(line, type_d, name, line_num)
            if self.omitting:
                self.omit_counter += 1
        elif not self.omitting and omit_start in line:
            self.omit_counter += 1
            self.omitting = True
        elif not self.omitting and omit_next in line:
            self.omitting_next = True
        elif self.omitting and omit_start in line:
            self.omit_counter += 1
        elif self.omitting and omit_end in line:
            self.omit_counter -= 1
            if self.omit_counter == 0:
                self.omitting = False
            elif self.omit_counter < 0:
                raise OmitInbalanceError(name, line_num)

        return (
            self.omitting or
            any(c in line for c in [omit_next, omit_start, omit_end, omit_cond])
        )



    def gen_single_source(self, name, newName, type_d, name_subs,
                          source_suffix, header_suffix):
        """
        This is the main parsing function to generate a file
        from a template
        """
        inF = open(name, "r")
        outF = open(newName, "w")
        mutating = False
        mutate = []
        line_num = 0
        outF.write(gen_template_intro_str(name, self.type_dict[type_d]))

        #if the type is an object and it needs to have any extra headers
        if((type_d in self.obj_dict ) and
           (
               (name.endswith(".ctemplate")) or
               (type_d not in self.refu_objects)
        )):

            #simply add them
            outF.write("/**\n"
                       " ** ---- Headers added by the building script specifically"
                       " for {} ----\n*/\n\n".format(self.type_dict[type_d]))
            for header in self.obj_dict[type_d]["headers"]:
                if type_d in self.refu_objects:
                    outF.write("#include <{}>\n".format(header))
                else:
                    if header.startswith("<"):
                        outF.write("#include {}\n".format(header))
                    elif name.endswith(".ctemplate"): # relative headers go only in source
                        outF.write("#include \"{}\"\n".format(header))

        #start creating the source from the generic template
        for line in inF:
            if self.parser_check_omits(line, type_d, name, line_num):
                continue
            #else we check for name substitution
            line = self.check_line_for_name_sub(line, name_subs, type_d,
                                                source_suffix, header_suffix)
            #we also check for mutation
            if mutating is True:
                mutating = False
                line = self.handle_mutate(mutate, type_d, line, name, line_num)
                mutate[:] = []
                #now proceed to write the line
            elif mutate_cmd in line:
                mutating = True
                line = line.replace(mutate_cmd, "")
                line = line.lstrip()
                l = line.split()
                for arg in l:
                    # if it's the end of the comment break
                    if "*/" in arg:
                        break
                    #else add a mutate argument
                    mutate.append(arg)
                continue
            #all writes happen here
            outF.write(line)
        inF.close()
        outF.close()
