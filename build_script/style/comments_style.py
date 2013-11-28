import os
import re
import sys

from textwrap import TextWrapper
from difflib import Differ






class CommentFormatter:
    def __init__(self, column_padding):
        self.desc_column = column_padding
        self.cr = re.compile("(?P<comment_opening>\\/\\*+\n)"
                        "(?P<comment_body>(.|\n(?!\\*\\/))*?)"
                        "\n *\\*+\\/\n"
                        "(?P<next_line>.*?\\()")

        # matches (and will remove) the rfoptional doxygen tag that is no longer used
        self.optional_re = re.compile("\\\\rfoptional\\{.*?\\}")

        self.param_re = re.compile("( *\\*+ )(?P<param_name>(@param\\[?.*?\\]? (\w|\\.)+?)|(@return))\s(?P<param_desc>.*?)(?=( *\\*+ @param)|( *\\*+ @return)|( *\\*+ @see)|( *\\*+\\/)|$)", flags=re.DOTALL)

        # should match newline plus the start of next line of comment
        self.stars_re = re.compile("(((\n)|(\r\n)|(\n\r)|(\r))|^)( *\\*+)(?!\\/)")

        self.cm_intro = "/**"
        self.cm_outro = "\n**/\n"

        self.wrapper = TextWrapper(
            width=80,
            initial_indent='',
            subsequent_indent=' **' + ' ' * (self.desc_column - 3),
            break_long_words=True,
            break_on_hyphens=True
        )

        self.diff = Differ()


    def process_generic_comment_lines(self, s):
        s = self.stars_re.sub("\n **", s)
        return s

    def process_parameter(self, param_name, param_desc):
        ret = " ** "
        ret += param_name
        length = len(ret)

        # Remove all newline and comment stars
        param_desc = self.stars_re.sub("", param_desc)
        # Remove any extra spaces
        param_desc = " ".join(param_desc.split())

        #perform the line wrapping
        self.wrapper.initial_indent = ret + " " * (self.desc_column - length)
        if self.desc_column - length <= 0:
            self.wrapper.initial_indent = "\n" + self.wrapper.subsequent_indent
        ret = self.wrapper.fill(param_desc)
        return ret + "\n"


    def process_comment(self, comment):
        comment = self.optional_re.sub("", comment)
        m = self.param_re.search(comment)

        if m is None:  #no parameters found for formatting
            return "{}{}{}".format(
                self.cm_intro,
                self.process_generic_comment_lines(comment),
                self.cm_outro
            )

        ret = self.cm_intro
        while m is not None:
            ret += self.process_generic_comment_lines(comment[:m.start()])
            ret += self.process_parameter(
                m.group("param_name"),
                m.group("param_desc")
            )
            comment = comment[m.end():]
            m = self.param_re.search(comment)

        # add the remaining comment if any or remove the last newline added if not
        if comment:
            ret += comment
        else:
            ret = ret[:-1]

        # finish up with the outro
        ret += self.cm_outro
        return ret

    def reformat_file_comments(self, file_name, replace_file=False):
        """Reformats the given file's comments depending on the options"""
        inF = open(file_name, "r")

        s = inF.read();
        inF.close()
        original_string = s[:]
        formatted_string = ""

        match = self.cr.search(s)
        while match is not None:
            formatted_string += s[:match.start("comment_opening")]
            formatted_string += self.process_comment(
                match.group("comment_body")
            )
            formatted_string += match.group("next_line")

            s = s[match.end():]
            match = self.cr.search(s)

        formatted_string += s

        if not replace_file:
            original_l = original_string.splitlines(True)
            formatted_l = formatted_string.splitlines(True)
            res = list(diff.compare(original_l, formatted_l))
            sys.stdout.writelines(res)
            return

        # else we replace the file
        print("Reformatting comments for \"{}\"".format(file_name))
        outF = open(file_name, "w")
        outF.write(formatted_string)
        outF.close()












