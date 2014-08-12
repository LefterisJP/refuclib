import subprocess
from subprocess import CalledProcessError
from time import sleep

Import('compiler')
system_attributes = {'endianess': 'BIG',
                     'longsize': 4}

# TODO: Alter the compiler call according to the compiler
compileCall = compiler+" -I../../include systemcheck.c -o systemcheck"

# compile the system check program
try:
    p = subprocess.Popen(compileCall,
                         cwd=".",
                         shell=True,
                         stdout=subprocess.PIPE)

    for line in p.stdout:
        print(line.decode("utf8"))

    # wait till the subprocess ends
    ret = p.poll()
    while ret is None:
        ret = p.poll()
    if ret != 0:
        print("*ERROR*:Could not compile the system check program.")
        for line in p.stderr:
            line = line.decode("utf8")
            print(line)
            Exit(1)

except CalledProcessError as err:
    print("*ERROR*:There was an error while invoking the compiler for the "
          "system check program")
    print("The compiler returned [{0}] and the output was:".format(
        err.returncode))
    print(err.output)
    Exit(1)

# a small delay between compiling and running
sleep(0.05)

# now run the compiled program to get the system attributes
linecount = 0
try:
    p = subprocess.Popen("./systemcheck",
                         cwd=".",
                         shell=True,
                         stdout=subprocess.PIPE)
    for line in p.stdout:
        num = int(line.decode("utf8"))
        # check for size of long int
        if linecount == 0:
            system_attributes['longsize'] = num
        elif linecount == 1:
            if num == 1:
                system_attributes['endianess'] = 'LITTLE'
        linecount = linecount + 1
    # wait till the subprocess ends
    ret = p.poll()
    while ret is None:
        ret = p.poll()
    if ret != 0:
        print("*ERROR*:Could not run the system check program.")
        Exit(1)
except CalledProcessError as err:
    print("*ERROR*:There was an error while invoking the system check program")
    Exit(1)

# finally return the figured out system attributes
Return('system_attributes')
