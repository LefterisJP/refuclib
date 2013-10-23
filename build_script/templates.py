import os

class TemplateFileError(Exception):
    def __init__(self, filename, msg):
        self.filename = filename
        self.msg = msg
    def __str__(self):
        return "Problem with template file \"{}\"\n{}".format(self.filename,
                                                              self.msg)


class TemplateGeneratedStructure():

    def determine_header_prefix(self):

        if self.copy_type != 'simple':
            return "{}_{}_{}".format(
                self.template.name, self.copy_type, self.type_symbol
            )
        else: # simple copy type
            return "{}_{}".format(self.template.name, self.type_symbol)

    def determine_old_header_prefix(self):
        if self.copy_type == "simple":
            return self.template.name
        else: # deep/shallow
            return "{}_{}".format(
                self.template.name, self.copy_type
            )

    def get_source_name(self):
        if self.type_symbol != 'generic':
            return "{}_{}".format(self.template.source_name, self.type_symbol)
        else:
            return self.template.source_name

    def get_node_name(self):
        if self.type_symbol != 'generic':
            return "{}_{}".format(self.template.node_name, self.type_symbol)
        else:
            return self.template.node_name

    def __init__(self, template, type_symbol, copy_type,
                 obj_dict, type_dict, refu_objects, lms_list):
        self.template = template
        self.type_symbol = type_symbol
        self.copy_type = copy_type
        self.old_header_prefix = self.determine_old_header_prefix()
        self.header_prefix = self.determine_header_prefix()
        self.is_lms = False
        self.type_source_name = type_dict[self.type_symbol]
        if type_symbol in lms_list:
            self.is_lms = True
        self.is_obj = False
        if type_symbol in obj_dict:
            self.is_obj = True
        self.is_refu_obj = False
        if type_symbol in refu_objects:
            self.is_refu_obj = True


class Template():

    def __init__(self, name, tfiles_dict, template_data_struct,
                 template_node, types_list, obj_dict, type_dict,
                 refu_objects, lms_list):
        self.name = name.lower()
        self.source_name = template_data_struct
        self.node_name = template_node
        self.structures = {}

        simple = tfiles_dict.get("simple")
        if simple:
            self.parent_dir = os.path.dirname(simple)
        else:
            deep = tfiles_dict.get("deep")
            if not deep:
                raise TemplateFileError(self.name, "Unrecognized template "
                                        "data structure type")
            self.parent_dir = os.path.dirname(deep)

        for type_symbol in types_list:
            #don't keep the no template_one
            if type_symbol == "no_template":
                continue
            d = {}
            for key, value in tfiles_dict.iteritems():
                # deep copy of POD makes no sense
                if key == "deep" and type_symbol not in obj_dict:
                    continue
                # generic type (void*) goes only with shallow templates
                elif type_symbol == "generic" and key != "shallow":
                    continue
                # object should not go for shallow data structures
                # Use generic (void*) for this
                elif(
                        type_symbol != 'generic' and
                        key == "shallow" and
                        type_symbol in obj_dict
                 ):
                     continue

                d[key] = TemplateGeneratedStructure(
                    self, type_symbol, key, obj_dict,
                    type_dict, refu_objects, lms_list
                )
            self.structures[type_symbol] = d

    def iterate(self):
        """
           A generator function that achieves iteration of all structures
           inside a template
        """
        for type_d, d, in self.structures.iteritems():
            for copy_type, structure in d.iteritems():
                yield structure



