from utils import build_msg

Import('env')

conf = Configure(env)

if not conf.CheckLibWithHeader('check', 'check.h', 'c'):
    build_msg("Need libcheck for unit tests")
    Exit(1)

env = conf.Finish()
Return('env')
