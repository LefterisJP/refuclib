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
        if self.copy_type != "simple":
            return "{}_{}_{}".format(
                self.name, self.copy_type, self.type_symbol
            )
        else: # simple copy type
            return "{}_{}".format(self.name, self.type_symbol)

    def determine_old_header_prefix(self):
        if self.copy_type != "simple":
            return "{}_{}".format(
                self.name, self.copy_type
            )
        else: # simple copy type
            return self.name

    def determine_source_prefix(self, obj_dict):
        if self.type_symbol in obj_dict:
            if self.copy_type == "deep":
                return "d"
            elif self.copy_type == "shallow":
                return "s"
        return ""

    def __init__(self, name, type_symbol, copy_type, obj_dict):
        self.name = name
        self.type_symbol = type_symbol
        self.copy_type = copy_type
        self.old_header_prefix = self.determine_old_header_prefix()
        self.header_prefix = self.determine_header_prefix()
        self.source_prefix = self.determine_source_prefix(obj_dict)
        
        


class Template():

    def __init__(self, name, tfiles_dict, types_list, obj_dict):
        self.name = name.lower()
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
            #don't keep the generic one 
            if type_symbol == "generic":
                continue
            d = {}
            for key, value in tfiles_dict.iteritems():
                # deep copy of POD makes no sense
                if key == "deep" and type_symbol not in obj_dict:
                    continue
                
                d[key] = TemplateGeneratedStructure(
                    self.name, type_symbol, key, obj_dict
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
            
            

