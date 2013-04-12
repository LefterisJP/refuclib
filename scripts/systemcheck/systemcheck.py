import subprocess
from subprocess import CalledProcessError
from time import sleep
import platform
 
Import('compiler')
systemAttributes = {'endianess':'BIG', 'longsize':4}

#TODO: Alter the compiler call according to the compiler
compileCall = compiler+" -I../../include systemcheck.c -o systemcheck"

    
#compile the system check program
try:
    p = subprocess.Popen(compileCall,cwd=".",shell=True,stdout=subprocess.PIPE)
    for line in p.stdout:
        print(line.decode("utf8"));
    #wait till the subprocess ends
    ret = p.poll();
    while(ret == None):
        ret=p.poll()
    if(ret!=0):
        print("*ERROR*:Could not compile the system check program.")
        for line in p.stderr:
            line = line.decode("utf8")
            print(line)

        #return False
except CalledProcessError as err:
    print("*ERROR*:There was an error while invoking the compiler for the system check program")
    print("The compiler returned [{0}] and the output was:".format(err.returncode))
    print(err.output);
    #return False;

#a small delay between compiling and running
sleep(0.05)
    
#now run the compiled program to get the system attributes
linecount = 0;
try:
    p = subprocess.Popen("./systemcheck",cwd=".",shell=True,stdout=subprocess.PIPE)
    for line in p.stdout:
        num = int(line.decode("utf8"))
        #check for size of long int
        if(linecount==0):
            systemAttributes['longsize'] = num
        elif(linecount==1):
            if(num==1):
                systemAttributes['endianess'] = 'LITTLE' 
        linecount = linecount+1;
    #wait till the subprocess ends
    ret = p.poll()
    while(ret == None):
        ret=p.poll()
    if(ret!=0):
        print("*ERROR*:Could not run the system check program.")
        #return False
except CalledProcessError as err:
    print("*ERROR*:There was an error while invoking the system check program")
    #return False;
    
#finally return the figured out system attributes
Return('systemAttributes');