import sys

from difflib import Differ

class Formatter():
    def __init__(self):
        self.diff = Differ()


    def reformat_file(self, file_name, style_target,
                      formatting_function, replace_file=False):
        """Reformats the given file's comments depending on the options"""
        inF = open(file_name, "r")

        original_string = inF.read();
        inF.close()
        formatted_string = formatting_function(original_string)

        if not replace_file:
            original_l = original_string.splitlines(True)
            formatted_l = formatted_string.splitlines(True)
            res = list(self.diff.compare(original_l, formatted_l))
            sys.stdout.writelines(res)
            return

        # else we replace the file
        print("Reformatting {} for \"{}\"".format(style_target, file_name))
        outF = open(file_name, "w")
        outF.write(formatted_string)
        outF.close()
