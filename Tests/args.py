import sys
from optparse import OptionParser

validCompilers = ['gcc','tcc']

def listsplit(option, opt, value, parser):
	"""This is the callback function used to split the comma separated arguments into lists"""
	setattr(parser.values, option.dest, value.split(','))


def parseArguments():
	""" Parses the arguments given to the framework from the command line"""
	info = "===Refu Library Automated Testing Framework==="
	parser  =OptionParser(description=info,version="Refu Testing framework version 1.0")

	parser.add_option("-v","--verbose",dest="verbose",default=False,action="store_true",
	help="The testing will be verbose and will output as many things as possible to the stdout")

	parser.add_option("-f","--logfile",dest="logfile",default="TestsLog",action="store",type="string",
	help="Provides the name of the lofgile to keep for all the tests. Default is \"TestsLog\"")

	parser.add_option("-d","--debug",dest="debug",default=False,action="store_true",
	help="This options runs all the tests without actually comparing them to the expected output but instead spitting it all to the stdout. Used for testing the tests")

	parser.add_option("-c","--compilers",type='string',action='callback',callback=listsplit,default=['gcc'],
	help="Provide a list of valid compiler names separated by commas to compile and run the tests with. Make sure to provide no spaces in between. Valid compiler values at the moment are: "+str(validCompilers))
	
	parser.add_option("-t","--tests",type='string',action='callback',callback=listsplit,default=[],
	help="Provide a list of specific tests to run. When this options is given all the tests are not performed but instead only the ones denoted as arguments are")

	(options, args) = parser.parse_args()
	
	if(not set(options.compilers).issubset(set(validCompilers))): 
		parser.error("One of the given compiler values is not a supported compiler name. Valid compiler names are: "+str(validCompilers))
	parser.destroy()
	return options;