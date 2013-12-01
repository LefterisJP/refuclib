#!/usr/bin/env python2.7
import os
import subprocess
import argparse
import sys
import fnmatch

from comments_style import CommentFormatter
from naming import NamingFormatter
from utils import Formatter

_dirname = os.path.dirname(os.path.realpath(__file__))
_diff_exe = "colordiff"

def gen_diff_call(file_name):
    if _diff_exe == "colordiff":
        return [_diff_exe,
                "--show-c-function",
                "-u",
                "{}.orig".format(file_name),
                file_name
            ]
    else:
        return [_diff_exe,
                "-u",
                "{}.orig".format(file_name),
                file_name
            ]


def parse_arguments():
    parser = argparse.ArgumentParser(description='Astyle wrapper script')
    parser.add_argument('-s', '--check-style', action="store_true",
                        help='checks for style violations')
    parser.add_argument('-f', '--format-style', action="store_true",
                        help='Reformats style')
    parser.add_argument('-c', '--check-comments', action='store_true',
                        help='checks the style of comments above functions'
                        ' and provides a diff of how they would be reformatted'
                        ' if the -C flag was passed')
    parser.add_argument('-C', '--format-comments', action='store_true',
                        help='reformats the style of comments above functions')

    parser.add_argument('-n', '--check-function-naming', action='store_true',
                        help='checks the style of function naming')
    parser.add_argument('-N', '--format-function-naming', action='store_true',
                        help='reformats the style of function naming')

    parser.add_argument('-t', '--check-type-naming', action='store_true',
                        help='checks the style of type naming')
    parser.add_argument('-T', '--format-type-naming', action='store_true',
                        help='reformats the style of type naming')


    parser.add_argument('-r', '--recursive', action='store_true',
                        help='Expects one or more target directories and '
                        'applies actions recursively to all source files under'
                        'them')

    parser.add_argument('--comment-column-padding', type=int,
                        default=34, help='The number of spaces to put as '
                        'padding right of parameters when reformatting '
                        'the comments')
    parser.add_argument(dest='targets', nargs='+',
                        help='One or more targets to apply style actions on')
    args = parser.parse_args()
    return args

def is_on_path(program):
    """Checks if an executable is on the path and can be executed"""
    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    for path in os.environ["PATH"].split(os.pathsep):
        path = path.strip('"')
        exe_file = os.path.join(path, program)
        if is_exe(exe_file):
            return True

    return False

def sanity_check():
    global _diff_exe
    """Makes sure we have the programs needed"""
    if not is_on_path("astyle"):
        print ("astyle can not be found in the path. Quitting ...")
        return False
    if not is_on_path("colordiff"):
        print ("colordiff can't be found in the path. Will use diff")
        _diff_exe = "diff"
    return True


def call_astyle(file_name):
    """
    Calls astyle with the arguments specified in the options file
    which should be in the same directory as the executable
    """
    if not os.path.isfile(file_name):
        print("{} could not be found".format(file_name))
        return False

    try:
        rc = subprocess.call([
            "astyle",
            "--options={}/astyle_options".format(_dirname),
            file_name]
        )
    except OSerror as e:
        if e.errno == os.errno.ENOENT:
            print("astyle could not be found in the path")
        else:
            print("There was a problem invoking the astyle command")
        return False

    if rc != 0:
        print("astyle command returned error")
        return False

    return True

def check_style(file_name):
    """Checks style violations and provides a diff"""
    if not call_astyle(file_name):
        return False

    if not os.path.isfile("{}.orig".format(file_name)):
        print("Woohoo! Go you! No style violations!")
        return True

    l = gen_diff_call(file_name)
    try:
        subprocess.call(l)
    except:
        print("There was a problem invoking the diff command")
        return False

    os.rename("{}.orig".format(file_name), file_name)
    return True




def reformat_with_astyle(file_name, keep_original=False):
    """Reformats the file using astyle"""
    print("Reformatting style of \"{}\"".format(file_name))

    if not call_astyle(file_name):
        return False

    if not keep_original:
        try:
            os.remove("{}.orig".format(file_name))
        except: # file does not get created if there was nothing to reformat
            pass





def apply_checks(args, file_name, formatter, comments, naming):
    """applies all style actions to a single file"""
    if args.check_style:
        check_style()

    if args.format_style:
        reformat_with_astyle(file_name)

    if args.check_comments or args.format_comments:
        formatter.reformat_file(
            file_name, "comments",
            comments.reformat_file_comments, args.format_comments
        )

    if args.check_function_naming or args.format_function_naming:
        formatter.reformat_file(
            file_name, "function naming",
            naming.reformat_function_naming, args.format_function_naming
        )

    if args.check_type_naming or args.format_type_naming:
        formatter.reformat_file(
            file_name, "type naming",
            naming.reformat_type_naming, args.format_type_naming
        )



if __name__ == '__main__':
    if not sanity_check():
        sys.exit(1)

    args = parse_arguments()
    comments = CommentFormatter(args.comment_column_padding)
    naming = NamingFormatter()
    formatter = Formatter()
    if not args.recursive:
        for target in args.targets:
            apply_checks(args, os.path.abspath(target),
                         formatter, comments, naming)
    else:
        matches = []
        for target in args.targets:
            target = os.path.abspath(target)
            if not os.path.isdir(target):
                print("Provided target \"{}\" is not a directory."
                      " Skipping".format(target))
                continue
            for root, dirnames, filenames in os.walk(target):
                for filename in filenames:
                    if filename.endswith(('.c', '.h', '.ph')):
                        matches.append(os.path.join(root, filename))

        for f in matches:
            apply_checks(args, f, formatter, comments, naming)

