import sys

import argparse

validCompilers = ['gcc','tcc']

def parseArguments():
    """ Parses the arguments given to the framework from the command line"""
    info = "===Refu Library Automated Testing Framework==="
    parser  = argparse.ArgumentParser(description=info)

    parser.add_argument("-v", "--verbose",
                        dest="verbose",
                        default=False,
                        action="store_true",
                        help="The testing will be verbose and will output "
                        "as many things as possible to the stdout")

    parser.add_argument("-f", "--logfile", dest="logfile",
                        default="TestsLog", action="store",
                        help="Provides the name of the lofgile to keep for"
                        "all the tests. Default is \"TestsLog\"")

    parser.add_argument("-d", "--debug", dest="debug", default=False,
                        action="store_true", help="This option runs all "
                        "the tests without actually comparing them to the "
                        "expected output but instead spitting it all to "
                        "the stdout. Used for testing the tests")

    parser.add_argument("-c", "--compilers", dest="compilers",
                        nargs='+',
                        default=['gcc'], help="Provide a list of valid "
                        "compiler names separated by spaces to compile and "
                        "run the tests with. Valid compiler values at the moment "
                        "are: "+str(validCompilers))

    parser.add_argument("-t", "--tests", dest="tests",
                        nargs='+' , default=[], help="Provide a "
                        "list of specific tests to run. When this options "
                        "is given all the tests are not performed but "
                        "instead only the ones denoted as arguments are")

    parser.add_argument("--fail-fast", dest="fail_fast", default=False,
                        action="store_true", help="When this flag is "
                        "specified then the execution of the tests will "
                        "stop as soon as a single failure or error is "
                        "detected")

    parser.add_argument("-k", "--keep-exec", dest="keep_exec", default=False,
                        action="store_true", help="When this is specified and "
                        "either a specific test has been provided for running or"
                        " fail fast has been requested the test executable is "
                        "not cleaned after a test has concluded")

    args = parser.parse_args()

    if(not set(args.compilers).issubset(set(validCompilers))):
        parser.error("One of the given compiler values is not a supported "
                     "compiler name. Valid compiler names are: " +
                     str(validCompilers))
    return args
