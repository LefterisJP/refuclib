from utils import build_msg


def CheckExecutable(context, executable):
    context.Message('Checking for existence of {} ...'.format(executable))
    path = WhereIs(executable)
    if not path:
        context.Result('FAIL!')
    else:
        context.Result('OK!')
    return path

Import('env')

conf = Configure(env, custom_tests={'CheckExecutable': CheckExecutable})

if not conf.CheckLibWithHeader('check', 'check.h', 'c'):
    build_msg("Need libcheck for unit tests")
    Exit(1)


if not conf.CheckExecutable('valgrind'):
    build_msg("Will not run unit tests via valgrind", "Warning", env)
    env.SetDefault(has_valgrind=False)
else:
    env.SetDefault(has_valgrind=True)

env = conf.Finish()
Return('env')
