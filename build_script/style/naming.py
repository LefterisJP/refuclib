import re

class NamingFormatter:
    def __init__(self):
        self.funcs_re = re.compile(
            r"rf(?P<rf_object>[A-Z][A-Za-z0-9]+?)_"
            "(?P<func>\w+?)"
            "(?=(\n|,|\(|$| ))")

        self.camel_re = re.compile(
            r"_*(?P<word>[A-Za-z0-9]+?)"
            "(?=[A-Z]|_|$|\(| )")

        self.types_re = re.compile(
            r"RF_(?P<rf_object>[A-Z][a-z]\w+?)"
            "(?=\*| |\n|$|;)")


    def camel_to_lower(self, fstring, start_with=""):
        return_string = start_with

        m = self.camel_re.search(fstring)
        while m is not None:
            return_string += m.group("word").lower()
            return_string += "_"
            fstring = fstring[m.end():]
            m = self.camel_re.search(fstring)
            
        return return_string[:-1]

    def reformat_function_naming(self, original_string):
        """Reformats the given file's function naming"""
        s = original_string[:]
        formatted_string = ""

        match = self.funcs_re.search(s)
        while match is not None:
            formatted_string += s[:match.start()]
            formatted_string += "rf_"
            formatted_string += match.group("rf_object").lower()
            formatted_string += self.camel_to_lower(match.group("func"), "_")

            s = s[match.end():]
            match = self.funcs_re.search(s)

        formatted_string += s
        return formatted_string


    def reformat_type_naming(self, original_string):
        """Reformats the given file's type naming"""
        s = original_string[:]
        formatted_string = ""

        match = self.types_re.search(s)
        while match is not None:
            formatted_string += s[:match.start()]
            formatted_string += "RF"
            formatted_string += self.camel_to_lower(match.group("rf_object"))

            s = s[match.end():]
            match = self.types_re.search(s)

        formatted_string += s
        return formatted_string

        

        

