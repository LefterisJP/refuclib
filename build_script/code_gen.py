import shutil
import os
import json
import sys
from time import gmtime, strftime

from templates import Template, TemplateGeneratedStructure
from ctemplate_parser import (
    TemplateParser, Template_FuncCall, TemplateParserError
)
from ctemplate_preprocessor import Template_Preprocessor
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


addinc_start = "/* @additional_includes start */"
addinc_end = "/* @additional_includes end */"


def replace_func_call(line, funcname, replace_str):
    part = line.partition(funcname + "(")
    clean_line = part[0]
    clean_line = clean_line + replace_str
    
    line = part[2]
    part = line.partition(")")

    clean_line += part[2]
    return clean_line

class CodeGen():
    """
       This class encapsulates all of the code generating functionality
    """

    refu_objects = [ "string" ]
    lms_list = ["string"]
    


    def __init__(self, refu_root):
        """
           Initializes the codegen object, either creating the object and
           types dicts and list or loading them from a file
        """

        self.functions = {
            "@OBJECT_INIT_DEFAULT": self.handle_object_init_default,
            "@OBJECT_COPY": self.handle_object_copy,
            "@OBJECT_DESTROY": self.handle_object_destroy,
            "@OBJECT_COMPARE": self.handle_object_compare,
            "@ASSIGN": self.handle_assign,
            "@SHALLOW_PTR_ARITHMETIC": self.handle_shallow_ptr_arithmetic,
        }

        # types that can be safely templated from within the library
        self.library_types = {
            "i": "int",
            "ui": "unsigned int",
            "i8": "int8_t",
            "ui8": "uint8_t",
            "i16": "int16_t",
            "ui16": "uint16_t",
            "i32": "int32_t",
            "ui32": "uint32_t",
            "i64": "int64_t",
            "ui64": "uint64_t",
            "string": "struct RFstring",
            "generic": "void*",
            "no_template": "no_template"
            }

        if os.path.isfile(os.path.join(refu_root, json_file_name)):
            self.load_data_from_json(json_file_name)
        else:
            self.type_dict = { 
            "i": "int",
            "ui": "unsigned int",
            "i8": "int8_t",
            "ui8": "uint8_t",
            "i16": "int16_t",
            "ui16": "uint16_t",
            "i32": "int32_t",
            "ui32": "uint32_t",
            "i64": "int64_t",
            "ui64": "uint64_t",
            "string": "struct RFstring",
            "generic": "void*",
            "no_template": "no_template"
            }

            self.obj_dict = {
                "string": {
                    "init_default_func": "",
                    "destroy_func": "rf_string_deinit",
                    "copy_func": "rf_string_copy_in",
                    "compare_func": "rf_string_equal",
                    "headers": [
                        "Definitions/imex.h", # for import export macro
                        "Definitions/defarg.h", # for default arguments
                        "Preprocessor/rf_tokens.h", #for defined library tokens
                        "Definitions/types.h",  #for fixed size data types
                        "String/rf_str_decl.h",  #for RFstring
                        "String/rf_str_core.h" #for Equal, Copy_IN and Deinit
                    ]  
                }
            }


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



    def handle_object_copy(self, line, structure, keyword, line_num):
        line = line.replace(
            keyword,
            self.obj_dict[structure.type_symbol]["copy_func"]
        )
        return line

    def handle_object_init_default(self, line, structure, keyword, line_num):
        """
           If the object does have a default initialization function then
           it is used. If not the line is totally omitted.
        """
        if self.obj_dict[structure.type_symbol].get("init_default_func"):
            line = line.replace(
                keyword,
                self.obj_dict[structure.type_symbol]["init_default_func"]
            )
        else:
            line = ""
        return line

    def handle_assign(self, line, structure, keyword, line_num):
        if structure.copy_type == 'shallow':
            call = self.Template_Parser.query(keyword, line_num)
            arg1 = call.args[0]
            arg2 = call.args[1]
            arg1_type = call.get_arg_type(arg1)
            arg2_type = call.get_arg_type(arg2)
            arg1_name = call.get_arg_name(arg1)
            arg2_name = call.get_arg_name(arg2)
            if structure.type_symbol != 'generic':
                if '*' in arg1_type and '*' in arg2_type:
                    line = replace_func_call(line, keyword,
                                             "*({}) = *({})".format(
                                                 arg1_name, arg2_name
                                             )
                    )
                elif '*' in arg1_type and '*' not in arg2_type:
                    line = replace_func_call(line, keyword,
                                             "*({}) = {}".format(
                                                 arg1_name, arg2_name
                                             )
                    )
                elif '*' not in arg1_type and '*' in arg2_type:
                    line = replace_func_call(line, keyword,
                                             "{} = *({})".format(
                                                 arg1_name, arg2_name
                                             )
                    )
                else:
                    line = replace_func_call(line, keyword,
                                             "{} = {}".format(
                                                 arg1_name, arg2_name
                                             )
                    )
            else: # generic (void*) case
                line = replace_func_call(line, keyword,
                                         "(char*)({} * sizeof(void*))"
                                         " = {};".format(arg1_name, arg2_name))
        else: #only used in shallow templates for now
            raise Exception #TODO
        return line

    def handle_shallow_ptr_arithmetic(self, line, structure, keyword, line_num):
        if structure.copy_type != 'shallow':
            raise Exception #todo

        call = self.Template_Parser.query(keyword, line_num)
        arg = call.args[0]
        arg_type = call.get_arg_type(arg)
        arg_name = call.get_arg_name(arg)


        if structure.type_symbol != 'generic':
            # If it's in a return don't forget to dereference
            if call.has_extra("IN_RETURN"):
                line = replace_func_call(line, keyword, "*({})".format(arg_name))
            else:
                line = replace_func_call(line, keyword, arg_name)
        else: #generic (void*) case
            line = replace_func_call(line, keyword,
                                     "(char*)({} * sizeof(void*)".format(
                                         arg_name)
            )
        return line

    def handle_object_destroy(self, line, structure, keyword, line_num):
        line = line.replace(
            keyword,
            self.obj_dict[structure.type_symbol]["destroy_func"]
        )
        return line

    def handle_object_compare(self, line, structure, keyword, line_num):
        call = self.Template_Parser.query(keyword, line_num)
        arg1 = call.args[0]
        arg2 = call.args[1]
        arg1_name = call.get_arg_name(arg1)
        arg2_name = call.get_arg_name(arg2)
        if (structure.copy_type == 'deep' or 
            (
                structure.copy_type == 'shallow' and
                structure.type_symbol in self.obj_dict
        )):
            line = line.replace(
                keyword,
                self.obj_dict[structure.type_symbol]["compare_func"]
            )
        else: #shallow and POD, means simple comparison of POD
            line = replace_func_call(line, keyword,
                                     "{} == {}".format(arg1_name, arg2_name
            ))
        return line

    def check_functions(self, line, structure, line_num):
        for key, value in self.functions.iteritems():
            if key + "(" in line:
                line = value(line, structure, key, line_num)
        return line


    def add_object(self, extra_dict, refu_dir):
        """
        Adds an object to the code generation.
        Look at args_before_config.py for the details
        """
        self.type_dict[extra_dict["name"]] = extra_dict["source_name"]

        self.obj_dict[extra_dict["name"]] = {
            # FUNCTION: void default_init_func(obj):
            # initializes an empty object.
            # obj         The object to initialize
            # Can be omitted
            "default_init_func": extra_dict.get("default_init_func"),
            # FUNCTION: void destroy(obj):
            # Destroys an object
            # obj         The object to destroy
            "destroy_func": extra_dict.get("destroy_func"),
            # FUNCTION: bool copy(destination, source)
            # Copies an object to another
            # destination  The object to copy to
            # source       The object to copy from
            # RETURN       True in success and false in failure
            "copy_func": extra_dict.get("copy_func"),
            # FUNCTION: bool compare(obj1, obj2)
            # Compares an object with another
            # obj1         The first object to compare
            # obj2         The second object to compare
            # RETURN       True if they are considered the same and false if not
            "compare_func": extra_dict.get("compare_func"),

            
            # A list of headers that the object needs to add them to the 
            # generated source file
            "headers": extra_dict.get("headers"),
            # The directory/directories where the files generated by the 
            # template will go. You need to give this only if you have provided
            # headers that are relatively include from those directories so 
            # that the template parser will know where to find stuff
            "dirs": extra_dict.get("dirs"),
            # The store type of the data structure that will be generated
            # for the object. Can be one of ['both', 'deep', 'shallow']
            "store_type": extra_dict.get("store_type")
        }
        #since a new object has been added save it to the json file
        fname = os.path.join(refu_dir, json_file_name)
        if os.path.isfile(fname):
           os.remove(fname)
        self.save_data_to_json(fname)


    def code_gen(self, template, structure, root, type_d, name_subs):
        """
            This is the main function that creates the 3 source files
            for the named template according to the specified parameters.
            If any errors happen they are reported by this function and then
            the exception is re-raised

            The function returns a list of the generated source files
        """
        try:
            #generate _decl.h
            self.gen_single_source(
                os.path.join(
                    root, "include",
                    template.parent_dir,
                    structure.old_header_prefix + "_decl.htemplate"),
                os.path.join(
                    root, "include",
                    template.parent_dir,
                    structure.header_prefix + "_decl.h"),
                type_d,
                name_subs,
                structure
            )

            #generate .h
            self.gen_single_source(
                os.path.join(
                    root, "include",
                    template.parent_dir,
                    structure.old_header_prefix + ".htemplate"),
                os.path.join(
                    root, "include",
                    template.parent_dir,
                    structure.header_prefix + ".h"),
                type_d,
                name_subs,
                structure
            )
            #generate .c
            self.gen_single_source(
                os.path.join(
                    root, "src",
                    template.parent_dir,
                    structure.old_header_prefix + ".ctemplate"),
                os.path.join(
                    root, "src",
                    template.parent_dir,
                    structure.header_prefix + ".c"),
                type_d,
                name_subs,
                structure,
                [
                    os.path.join(root, "include"),
                    os.path.join(root, "src", template.parent_dir)
                ]
            )

            #remember the source from parent dir and down
            return os.path.join(template.parent_dir,
                                structure.header_prefix + ".c"
            )


        except (TemplateParserError)as err:
            print type(err)
            print(err)
            raise err

        return generated_sources

    def gen_single_source(self, name, newName, type_d,
                          name_subs, structure, incl_dirs=None):
        """
        This is the main parsing function to generate a file
        from a template
        """
        preprocessor = Template_Preprocessor(name, structure)
        pf = preprocessor.preprocess(
            name,
            self.obj_dict[structure.type_symbol]["headers"] if self.obj_dict.get(structure.type_symbol) else None,
            name_subs
        )

        pf.flush()

        # we only need to parse the AST for sources
        # since that's when we need some meta information
        # about the code to know how to replace stuff
        if name.endswith('.ctemplate'):
            extra_dirs = None
            if type_d in self.obj_dict:
                extra_dirs = self.obj_dict[type_d].get("dirs")
            self.Template_Parser = TemplateParser(pf.name, incl_dirs,
                                                  extra_dirs)
            self.Template_Parser.process()
        

        inF = open(pf.name, "r")
        outF = open(newName, "w")
        line_num = 1
        outF.write(gen_template_intro_str(name, structure.type_source_name))

        for line in inF:
            line = self.check_functions(line, structure, line_num)

            #all writes happen here
            outF.write(line)
            line_num += 1
        inF.close()
        outF.close()
