import sys

# TODO:
# this file should slowly encapsulate all the outputting and reporting
# functionality of the testing framework

def print_nonl(msg):
    sys.stdout.write(msg)
    sys.stdout.flush()
