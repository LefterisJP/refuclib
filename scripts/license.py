import os
import fnmatch
import sys

license_string = (
    "/**\n * @author: Lefteris Karapetsas\n"
    "* @licence: BSD3 (Check repository root for details)\n */\n"
)


def find_files(dir):
    matches = []
    for root, dirnames, filenames in os.walk(dir):
        for filename in fnmatch.filter(filenames, '*.[ch]'):
            matches.append(os.path.join(root, filename))
    return matches


def write_license(file):
    file.write(license_string)


def replace_single_file(file):
        print("Adding license in {} ...".format(file))
        with open(file, "r") as inF:
            data = inF.readlines()
            outF = open(file, "w")
            after_initial_comment = False
            in_initial_comment = False
            for index, line in enumerate(data):
                # Assume that the file begins with a /** comment
                # and that comment is the license string to replace
                if not after_initial_comment:
                    if line.startswith("/**"):
                        in_initial_comment = True
                        continue
                    elif line.endswith("*/"):
                        in_initial_comment = False
                        after_initial_comment = True
                        write_license(outF)
                        continue
                    elif not in_initial_comment:
                        outF.write(line)
                else:
                    outF.write(line)
            outF.close()


def replace_license(project_root):
    files = find_files(os.path.join(project_root, 'src'))
    files += find_files(os.path.join(project_root, 'include'))
    for f in files:
        replace_single_file(f)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print "ERROR: Expected only one argument. The root directory of the project."
        exit(1)
    replace_license(sys.argv[1])
