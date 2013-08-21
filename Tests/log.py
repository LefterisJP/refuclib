import os

from output import report, TestError
from  __builtin__ import any as in_any

acceptable_log_entries =  {
'dynamicarray': ['Attempted to set an element in an array with an index '
                 'out of bounds. If you want to add objects to the list '
                 'use the Add function.',
                 'Attempted to retrieve an element from an array '
                 'with an index that is out of bounds'
             ]
}

def check_log_line_acceptable(line, test_name):
    """ Checks if line is an acceptable log line 
        for test named test_name
    """
    try:
        acceptable_lines = acceptable_log_entries[os.path.basename(
            test_name)]
    except: #if it's not in the acceptable list at all 
        return False
    if acceptable_lines is None:
        return False
    return in_any(line in l for l in acceptable_lines)

def  check_library_log(log_file, root, test_name):
    """
       Copies the library log file into the output and into the 
       tests log if and only if there is anything to report

       log_file - The file to which the tests log is saved
       root - The root directory where the current test is ran

       If there is any error to report this will also raise
       a Test Error
    """
    refu_error_logf = os.path.join(root, "refuclib.log")
    if(os.path.getsize(refu_error_logf) > 0):
        report(log_file, "--Start of Library Log--")
        l = open(refu_error_logf)
        for line in l:
            if check_log_line_acceptable(line, test_name):
                continue
            report(log_file, line)
        report(log_file, "--End of Library Log--")
        raise TestError(test_name)
