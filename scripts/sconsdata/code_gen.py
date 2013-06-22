import shutil
import os

def enum(*sequential, **named):
    enums = dict(zip(sequential, range(len(sequential))), **named)
    return type('Enum', (), enums)

obj = enum("destroy", "copy", "compare", "headers")

type_dict = { "I": "int",
              "UI": "unsigned int",
              "I8": "int8_t",
              "UI8": "uint8_t",
              "I16": "int16_t",
              "UI16": "uint16_t",
              "I32": "int32_t",
              "UI32": "uint32_t",
              "I164": "int64_t",
              "UI64": "uint64_t",
              "String": "RF_String"
}

obj_dict = {
    "String": ["rfString_Deinit", "rfString_Copy_IN", "rfString_Equal",
               ["RFstring.h"]]
}

class MutateTargetError(Exception):
    def __init__(self, target_type, file_name, line):
        self.target_type = target_type
        self.file_name = file_name
        self.line = line
    def __str__(self):
        return ("Unrecognized mutation target type \"{}\" found in the "
        "source of {} at \n\"{}\"".format(
            self.target_type, self.file_name, self.line)
        )

class MutateSourceError(Exception):
    def __init__(self, source_type, file_name, line):
        self.source_type = source_type
        self.file_name = file_name
        self.line = line
    def __str__(self):
        return ("Unrecognized mutation source type \"{}\" found in the "
        "source of {} at \n\"{}\"".format(
            self.source_type, self.file_name, self.line)
        )

class MutateError(Exception):
    def __init__(self, msg, file_name, line):
        self.file_name = file_name
        self.msg = msg
        self.line = line
    def __str__(self):
        return ("Mutation Error: \"{}\" "
                "in {} at \n\"{}\"".format(
                    self.msg, self.file_name, self.line)
        )                 

class OmitConditionError(Exception):
    def __init__(self, condition, file_name, line):
        self.condition = condition
        self.file_name = file_name
    def __str__(self):
        return ("Unrecognized omit condition \"{}\" found in the"
        "source of {} at \n\"{}\"".format(
            self.condition, self.file_name, self.line)
        )

omit_start = "/* @omit start */"
omit_cond = "/* @omitcond "
omit_end = "/* @omit end */"
omit_next = "/* @omit next */"
mutate_cmd = "/* @mutate"

addinc_start = "/* @additional_includes start */"
addinc_end = "/* @additional_includes end */"
    
def code_gen(name, parent_dir, root, type_d, name_subs):
    """
        This is the main function that creates the 3 source files
        for the named template according to the specified parameters.
        If any errors happen they are reported by this function and then
        the exception is re-raised
    """
    try:
    #generate _decl.h
        gen_single_source(os.path.join(root, "include", parent_dir,
                                       name + "_decl.h"),
                          os.path.join(root, "include", parent_dir,
                                       name + "_" + type_d + "_decl.h"),
                          type_d,
                          name_subs,
        )
    #generate .h
        gen_single_source(os.path.join(root, "include", parent_dir,
                                           name + ".h"),
                              os.path.join(root, "include", parent_dir,
                                           name + "_" + type_d + ".h"),
                              type_d,
                              name_subs,
            )
        #generate .c
        gen_single_source(os.path.join(root, "src", parent_dir,
                                       name + ".c"),
                          os.path.join(root, "src", parent_dir,
                                       name + "_" + type_d + ".c"),
                          type_d,
                          name_subs,
        )
    except (MutateError, MutateSourceError, MutateTargetError,
            OmitConditionError) as err:
        print type(err)
        print(err)
        raise err
    except Exception as err:
        print("Unknown Problems during code generation. Error type:")
        print type(err)
        raise err

def check_line_for_name_sub(line, subs, type_d):
    for s in subs:
        if s in line:
            line = line.replace(s, s + "_" + type_d)
    return line

def get_func1_args(line, func_name):
    """
       Returns the argument of an 1 argument function
    """
    line = line.partition(func_name+"(")[2]
    arg = line.partition(")")[0]
    arg = arg.strip()
    return arg

def get_func2_args(line, func_name):
    """
       Returns a tuple with the arguments of a 2 arguments function
    """
    line = line.partition(func_name+"(")[2]
    part = line.partition(",")
    arg1 = part[0].strip()
    arg2 = part[2].partition(")")[0]
    arg2 = arg2.strip()
    return (arg1, arg2)

def handle_COMPARE(line, mutate, type_d, file_name):
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
        if type_d in obj_dict:
            if mutate[0] == "ptr2Equalpr":
                return "if({}({}, &{}))\n".format(obj_dict[type_d][obj.compare], 
                                                  arg1,
                                                  arg2)
            elif mutate[0] == "ptr2Equalpp":
                return "if({}({}, {}))\n".format(obj_dict[type_d][obj.compare], 
                                                 arg1,
                                                 arg2)
            elif mutate[0] == "ptr2Equalrr":
                return "if({}(&{}, &{}))\n".format(obj_dict[type_d][obj.compare], 
                                                   arg1,
                                                   arg2)
            elif mutate[0] == "ptr2Equalrp":
                return "if({}(&{}, {}))\n".format(obj_dict[type_d][obj.compare], 
                                                  arg1,
                                                  arg2)
            else:
                raise MutateSourceError(mutate[0], file_name, line)
        else:
            return "if({} == {})\n".format(arg1, arg2)
    else:
        raise MutateSourceError(mutate[0], file_name, line)

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

def handle_ASSIGN(line, mutate, type_d, file_name):
    """
       Performs the mutation of the generic template code for
       copying objects. It either gets changed into:
       - If simple data types then plain old data assignment
       - If an object, then that objects's copy function
    """
    if("ptr2Copy" in mutate[0]):
        (arg1, arg2) = get_func2_args(line, "ptr2Copy")
        if type_d in obj_dict:
            if mutate[0] == "ptr2Copyrr":
                return "{}(&{}, &{});\n".format(obj_dict[type_d][obj.copy],
                                          arg1,
                                          arg2)
            elif mutate[0] == "ptr2Copypp":
                return "{}({}, {});\n".format(obj_dict[type_d][obj.copy],
                                          arg1,
                                          arg2)
            elif mutate[0] == "ptr2Copypr":
                return "{}({}, &{});\n".format(obj_dict[type_d][obj.copy],
                                          arg1,
                                          arg2)
            elif mutate[0] == "ptr2Copyrp":
                return "{}(&{}, {});\n".format(obj_dict[type_d][obj.copy],
                                          arg1,
                                          arg2)
            else:
                raise MutateSourceError(mutate[0], file_name, line)
        else:
            return "{} = {};\n".format(arg1, arg2)
    else:
        raise MutateSourceError(mutate[0], file_name, line)

def handle_DESTROY(line, mutate, type_d, file_name):
    """
       Performs the mutation of the generic template code for
       destroying objects. It either gets changed into:
       - If simple data types then omit
       - If an object, then that objects's destroy function
    """
    if("ptr2Destroy" in mutate[0]):
        arg = get_func1_args(line, "ptr2Destroy")
        if type_d in obj_dict:
            if mutate[0] == "ptr2Destroyr":
                return "{}(&{});\n".format(obj_dict[type_d][obj.destroy], 
                                           arg)
            elif mutate[0] == "ptr2Destroyp":
                return "{}({});\n".format(obj_dict[type_d][obj.destroy], 
                                           arg)
            else:
                raise MutateSourceError(mutate[0], file_name, line)
        else:
            return "";

def handle_INITIALIZE(line, mutate, type_d, file_name):
    """
       If type is not plain old data, mutate[0] (mutate_from) is changed
       into a compound literal structure initialization
    """
    if type_d in obj_dict:
        return line.replace(mutate[0], "(const struct {}) {{0}}".format(
            type_dict[type_d]))
    else:
        return line

def handle_REFERENCE(line, mutate, type_d, file_name):
    """
       If type is not plain old data, mutate[0] (mutate_from) is changed
       into a reference
    """
    if type_d in obj_dict:
        return line.replace(mutate[0],"&"+mutate[0] )
    else:
        return line

def handle_TYPE(line, mutate, type_d, file_name):
    """
       mutate[0] (mutate_from) is changed into the type
    """
    return line.replace(mutate[0], type_dict[type_d])

def handle_SIZE(line, mutate, type_d, file_name):
    """
       mutate[0] (mutate_from) is changed into the size of the type
    """
    return line.replace(mutate[0], "sizeof({})".format(
        type_dict[type_d]))

def handle_TYPEPTR(line, mutate, type_d, file_name):
    """
       mutate[0] (mutate_from) is changed into the type if the type is
       plain old data. If not then it's changed into type pointer
    """
    if type_d in obj_dict:
        return line.replace(mutate[0], type_dict[type_d]+"*")
    else:
        return line.replace(mutate[0], type_dict[type_d])

def handle_REMOVE(line, mutate, type_d, file_name):
    """
        Handle the simple case of requesting removal of something
    """
    return line.replace(mutate[0], "")

def handle_mutate(mutate, type_d, line, name):
    if len(mutate) < 2:
        raise MutateError("mutate list with less than 2 arguments passed "
                          "to handler", name, line)
    #if the 2nd argument to mutate is unknown we got a problem
    if mutate[1] not in handle_dict:
        raise MutateTargetError(mutate[1], name, line)
    # handle any extra arguments first
    line = check_extra_args(line, mutate, type_d)
    # and then call the "main" handler
    return handle_dict[mutate[1]](line, mutate, type_d, name)

def handle_omit_cond(line, type_d, filename):
    """
       Handles omits with conditions
    """
    line = line.partition(omit_cond)[2]
    cond = line.partition(" ")[0]
    if cond == "OBJECT":
        if type_d in obj_dict:
            return True;
        return False;
    elif cond == "POD":
        if type_d not in obj_dict:
            return True;
        return False;
    else:
        raise OmitConditionError(cond, filename, line)
        

handle_dict = {
    "TYPE": handle_TYPE,
    "TYPEPTR": handle_TYPEPTR,
    "SIZE": handle_SIZE,
    "REFERENCE": handle_REFERENCE,
    "ASSIGN": handle_ASSIGN,
    "COMPARE": handle_COMPARE,
    "DESTROY": handle_DESTROY,
    "INITIALIZE": handle_INITIALIZE,
    "REMOVE": handle_REMOVE
};

def gen_single_source(name, newName, type_d, name_subs):
    inF = open(name, "r")
    outF = open(newName, "w")
    omitting = False
    omitting_next = False
    mutating = False
    mutate = []
    #if the type is an object and it needs to have any extra headers
    if type_d in obj_dict and name.endswith("_decl.h"):
        #simply add them
        for header in obj_dict[type_d][obj.headers]:
            outF.write("#include <{}>\n".format(header))
    #start creating the source from the generic template
    for line in inF:
        if omitting_next:
            omitting_next = False
            continue
        if omit_cond in line:
            if handle_omit_cond(line, type_d, name) == True:
                omitting = True
                continue
        if not omitting and omit_start in line:
            omitting = True
            continue
        elif not omitting and omit_next in line:
            omitting_next = True
            continue
        elif omitting and omit_end in line:
            omitting = False
            continue
        elif omitting:
            continue
        #else we check for name substitution
        line = check_line_for_name_sub(line, name_subs, type_d)
        #we also check for mutation
        if mutating is True:
            mutating = False
            line = handle_mutate(mutate, type_d, line, name)
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
