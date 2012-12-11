#include <RFio.h>
#include <refu.h>

int main()
{
    rfInit("errorLog","infoLog");
    char buff[1024];
    //open a pipe with ls command
    FILE* p = rfPopen(RFS_("ls"),"r");
    if(p == 0)
        return -1;
    //print the stdout of the pipe
    while(fgets(buff,1024,p)!= 0)
    {
        printf("%s",buff);
    }
    //close the pipe
    rfPclose(p);
    return 0;
}
