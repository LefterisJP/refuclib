import shutil
import os

type_dict = { "I": "int",
              "UI": "unsigned int"
}

class MutateTargetError(Exception):
    def __init__(self, target_type, file_name):
        self.target_type = target_type
        self.file_name = file_name
    def __str__(self):
        return ("Unrecognized mutation target type \"{}\" found in the"
        "source of {}".format(self.target_type, self.file_name))

class MutateSourceError(Exception):
    def __init__(self, source_type, file_name):
        self.source_type = source_type
        self.file_name = file_name
    def __str__():
        return ("Unrecognized mutation source type \"{}\" found in the"
        "source of {}".format(self.target_type, self.file_name))

omit_start = "/* @omit start */"
omit_end = "/* @omit end */"
omit_next = "/* @omit next */"
mutate_cmd = "/* @mutate"

addinc_start = "/* @additional_includes start */"
addinc_end = "/* @additional_includes end */"
    
def code_gen(name, parent_dir, root, type_d, name_subs):
    #generate _decl.h
    gen_single_source(os.path.join(root, "include", parent_dir,
                                   name + "_decl.h"),
                      os.path.join(root, "include", parent_dir,
                                   name + type_d + "_decl.h"),
                      type_d,
                      name_subs,
    )
    #generate .h
    gen_single_source(os.path.join(root, "include", parent_dir,
                                   name + ".h"),
                      os.path.join(root, "include", parent_dir,
                                   name + type_d + ".h"),
                      type_d,
                      name_subs,
    )
    #generate .c
    gen_single_source(os.path.join(root, "src", parent_dir,
                                   name + ".c"),
                      os.path.join(root, "src", parent_dir,
                                   name + type_d + ".c"),
                      type_d,
                      name_subs,
    )


def check_line_for_name_sub(line, subs, type_d):
    for s in subs:
        if s in line:
            line = line.replace(s, s + type_d)
    return line

def get_func2_args(line, func_name):
    line = line.partition(func_name+"(")[2]
    part = line.partition(",")
    arg1 = part[0].strip()
    arg2 = part[2].partition(")")[0]
    arg2 = arg2.strip()
    return (arg1, arg2)

def handle_COMPARE(line, mutate_from, type_d, file_name):
    if(mutate_from == "ptr2Equal"):
        (arg1, arg2) = get_func2_args(line, mutate_from)
        return "if({} == {})\n".format(arg1, arg2)
    else:
        raise MutateSourceError(mutate_from, file_name)

def handle_ASSIGN(line, mutate_from, type_d, file_name):
    if(mutate_from == "ptr2Copy"):
        (arg1, arg2) = get_func2_args(line, mutate_from)
        return "{} = {};\n".format(arg1, arg2)
    else:
        raise MutateSourcError(mutate_from, file_name)

def handle_FUNCNAME(line, mutate_from, type_d, file_name):
    if mutate_from == "rmi":
        return line.replace("i_rf", "rf")
    else:
        raise MutateSourcError(mutate_from, file_name)

def handle_TYPE(line, mutate_from, type_d, file_name):
    return line.replace(mutate_from, type_dict[type_d])

def handle_mutate(mutate_to ,mutate_from, type_d, line, name):
    if mutate_to not in handle_dict:
        raise MutateTargetError(mutate_to, name)
    return handle_dict[mutate_to](line, mutate_from, type_d, name)

handle_dict = {
    "TYPE": handle_TYPE,
    "ASSIGN": handle_ASSIGN,
    "COMPARE": handle_COMPARE,
    "FUNCNAME": handle_FUNCNAME,
};

def gen_single_source(name, newName, type_d, name_subs):
    inF = open(name, "r")
    outF = open(newName, "w")
    omitting = False
    omitting_next = False
    mutating = False
    mutate_from = ""
    mutate_to = ""
    for line in inF:
        if omitting_next:
            omitting_next = False
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
            line = handle_mutate(mutate_to, mutate_from,
                                 type_d, line, name)
            #now proceed to write the line
        elif mutate_cmd in line:
            mutating = True
            line = line.replace(mutate_cmd, "")
            line = line.lstrip()
            l = line.split()
            mutate_from = l[0]
            mutate_to = l[1]
            continue
        #all writes happen here
        outF.write(line)
    inF.close()
    outF.close()
