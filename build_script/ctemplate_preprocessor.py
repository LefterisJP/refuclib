import tempfile

class Template_Preprocessor:
    def __init__(self, filename, structure):
        self.filename = filename
        self.structure = structure

        self.keywords = [
            "@TYPEPTR_SHALLOW",
            "@TYPEPTR",
            "@TYPE",
            "@OBJECT_SIZE",
            "@DATA_STRUCT",
            "@NODE"
        ]
        self.keywords_dict = {
            "@TYPEPTR_SHALLOW": self.handle_typeptr_shallow,
            "@TYPEPTR": self.handle_typeptr,
            "@TYPE": self.handle_type,
            "@OBJECT_SIZE": self.handle_object_size,
            "@DATA_STRUCT": self.handle_data_struct,
            "@NODE":self.handle_node
        }




    def check_line_for_name_sub(self, line, subs):
        """
        Replaces the name subs from the template for this line.
        If it's a generic data structure there is no need to change
        the names
        """
        if self.structure.type_symbol == "generic":
            return line

        for s in subs:
            #if it's an include line DO NOT replace anything
            if "#include" in line:
                return line
            if s in line:
                line = line.replace(s, s + "_" + self.structure.type_symbol)
        return line

    def handle_type(self, line, keyword):
        """
           @TYPE
           The type itself
        """
        if self.structure.copy_type == 'shallow':
            raise Exception #TODO
        line = line.replace(keyword, self.structure.type_source_name)
        return line

    def handle_typeptr(self, line, keyword):
        """
            @TYPEPTR
            - Deep copy: Pointer to object
            - Shallow copy:
                   +POD: itself
                   +Generic: void*
        """
        if self.structure.copy_type == 'deep':
            line = line.replace(keyword, "{}*".format(
                self.structure.type_source_name
            ))
        elif self.structure.copy_type == 'shallow':
            if self.structure.is_obj:
                line = line.replace(keyword, "void*")
            else:
                line = line.replace(
                    keyword,
                    self.structure.type_source_name
                )
        else:
            raise Exception #TODO
        return line

    def handle_typeptr_shallow(self, line, keyword):
        """
            @TYPEPTR_SHALLOW
            - Deep copy: ERROR
            - Shallow copy:
                  +POD: Pointer to the POD
                  +Generic: void*
        """
        if self.structure.copy_type == 'deep':
            raise Exception #TODO

        if self.structure.is_obj:
            line = line.replace(keyword, "void*")
        else:
            line = line.replace(keyword, "{}*".format(
                    self.structure.type_source_name
            ))
        return line

    def handle_object_size(self, line, keyword):
        if self.structure.copy_type == 'deep':
            line = line.replace(keyword, "sizeof({})".format(
                self.structure.type_source_name
            ))
        elif self.structure.copy_type == 'shallow':
            if not self.structure.is_obj:
                line = line.replace(keyword, "sizeof({})".format(
                    self.structure.type_source_name
                ))
            else:
                line = line.replace(keyword, "sizeof(void*)")
        else:
            raise Exception #TODO
        return line


    def handle_data_struct(self, line, keyword):
        return line.replace(keyword, self.structure.get_source_name())

    def handle_node(self, line, keyword):
        return line.replace(keyword, self.structure.get_node_name())


    def check_keywords(self, line):
        """
        TODO: Find a better way to do this. This depends totally
        on the order I have defined keyword in the dict since some
        keywords are substrings of some others
        """
        for keyword in self.keywords:
            if keyword in line:
                line = self.keywords_dict[keyword](line, keyword)
        return line

    def process_include(self, line):
        """
           For now it only works by changing the whole line
        """
        part = line.partition('$INCLUDE')
        keyword = part[2].strip()
        if keyword == '@H':
            line = "#include <Data_Structures/{}.h>\n".format(
                self.structure.header_prefix
            )
        elif keyword == '@DECL_H':
            line = "#include <Data_Structures/{}_decl.h>\n".format(
                self.structure.header_prefix
            )
        else:
            raise Exception #TODO
        return line

    def process_if(self, line, keyword):
        """
            Processed the IF condition and returns True and False depending
            on whether we should include it in the final file
        """
        part = line.partition(keyword)
        condition = part[2].strip()
        if condition == '1' or condition == 'TRUE':
            ret = True
        elif condition == '0' or condition == 'FALSE':
            ret = False
        elif condition == '@LMS':
            if self.structure.is_lms:
                ret = True
            else:
                ret = False
        elif condition == "@STRING":
            if self.structure.type_symbol == 'String':
                ret = True
            else:
                ret = False
        else:
            raise Exception #TODO

        if keyword == '$IFNOT':
            ret = not ret
        return ret

    def add_extra_includes(self, outF, name, headers):
        """
           Adds extra includes if necessary
        """
        if( headers and (self.structure.is_obj ) and
        (
            (name.endswith(".ctemplate")) or
            (not self.structure.is_refu_obj)
        )):
            #simply add them
            outF.write("/**\n"
                       " ** ---- Headers added by the building script specifically"
                       " for {} ----\n*/\n\n".format(
                           self.structure.get_source_name()
                       )
            )
            for header in headers:
                if self.structure.is_refu_obj:
                    outF.write("#include <{}>\n".format(header))
                else:
                    if header.startswith("<"):
                        outF.write("#include {}\n".format(header))
                    elif name.endswith(".ctemplate"): # relative headers go only in source
                        outF.write("#include \"{}\"\n".format(header))


    def preprocess(self, original_filename, extra_headers, name_subs):
        tempf = tempfile.NamedTemporaryFile()
        inF = open(self.filename)
        omitting = False

        self.add_extra_includes(tempf, original_filename, extra_headers)

        for line in inF:
            if "$INCLUDE" in line:
                line = self.process_include(line)
            elif "$IFNOT" in line:
                omitting = not self.process_if(line, '$IFNOT')
                continue
            elif "$IF" in line:
                omitting = not self.process_if(line, '$IF')
                continue
            elif '$ENDIF' in line:
                omitting = False
                continue

            if not omitting:
                line = self.check_keywords(line)
                line = self.check_line_for_name_sub(line, name_subs)
                tempf.write(line)

        return tempf

