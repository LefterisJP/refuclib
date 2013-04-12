#the maximum number of arguments for a function
maxArgs = 36

#stuff to import to be able to open files in the parent directory
import os.path
import sys

print("Generating rf_xmacro_decl.h ...");
f = open(os.path.dirname(sys.argv[0]) + "/../includePreprocessor/rf_xmacro_decl.h","w");


#put some required definitions at the top
f.write("/**\n** @file rf_xmacro_decl.h\n** @author Lefteris\n** @date 13/02/2012\n**\n\
** This file contains macros to easily declare functions with default arguments\n\
** in header files using the C preprocessor. It is automatically generated\n\
** by the python script gen_rf_xmacro_decl.py\n");
f.write("*/");


f.write("\n#include \"rf_xmacro_sub.h\"//to be able to do subtraction with the preprocessor\n")
f.write("#include \"rf_xmacro_gt.h\"//to be able to do greater than comparisons with the preprocessor\n")
f.write("#include \"rf_xmacro_lt.h\"//to be able to do less than comparisons with the preprocessor\n\n")
f.write("#include \"rf_xmacro_utils.h\"//to use util macros for token pasting\n");


#Write the macros used to pick the correct macro for function definition
f.write("\n\n\n///These macros here are used in order to pickup the correct function macro definition\n")
f.write("#define i_RP_DECL_CHECK2(A__,B__)   RP_GT(B__,A__)\n")
f.write("#define i_RP_DECL_CHECK1(A__,B__)   RP_LT(B__,A__)\n")
f.write("#define i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,thisArgN__)  i_RP_DECL_CHECK1(RP_SUB(maxArgsN__,defArgsN__),thisArgN__)\n");
f.write("#define i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,thisArgN__)  i_RP_DECL_CHECK2(RP_SUB(maxArgsN__,defArgsN__),thisArgN__)\n");
f.write("#define i_RP_PICK_FUNC_DECL(funcmacro__,eqresult__,cmpresult__)  i_RP_PASTE3(funcmacro__,eqresult__,cmpresult__)\n\n");


###############################################################################################################
#Now Write the big bulk of the macros. Namely those which are used to declare functions with default arguments
####################################################################################################################
i = maxArgs;
f.write("\n//! These macros are used when you want to declare a function in a header file with default arguments and want to avoid lots of typing\n");
while(i>0):
    f.write("\n\n\n//! This macro is used to declare a function with "+str(i)+" arguments in a header file\n");
    #0)write the macro which actually declares the functions
    f.write("#define RF_DECLARE_DFUNC"+str(i)+"(retType__,funcNarf__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+")\\\n");
    #now call the picking macro
    f.write("i_RP_DECLARE_FUNC"+str(i)+"(funcNarf__,retType__,"+str(i)+",defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+")\n\n");
    #0)write the general definition which picks up the rest
    f.write("#define i_RP_DECLARE_FUNC"+str(i)+"(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+")\\\n");
    #now call the macro which picks the appropriate macro definition
    f.write("i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC"+str(i)+"_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,"+str(i)+")");
    f.write(",i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,"+str(i)+"))(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+")\n\n");
    #1) Definition 1 if if compulsory number of arguments is equal to current
    f.write("#define i_RP_DECLARE_FUNC"+str(i)+"_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+")\\\n");
    #now declare the function
    f.write("retType__ funcNarf__##"+str(i)+"(");
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+");\n");

    #2) Definition 2,  if compulsory number of arguments is less than current
    f.write("#define i_RP_DECLARE_FUNC"+str(i)+"_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+")\\\n");
    #now declare the function
    f.write("retType__ funcNarf__##"+str(i)+"(");
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+");\\\n");
    #also call the declaration macro for lower arguments number
    if(i!=1):
        f.write("i_RP_DECLARE_FUNC"+str(i-1)+"(funcNarf__,retType__,maxArgsN__,defArgsN__,");
        j=1;
        while(j< i-1):
            f.write("arg__"+str(j)+",");
            j+=1;
        f.write("arg__"+str(j)+")\n");
    else:
        f.write("i_RP_DECLARE_FUNC"+str(i-1)+"(funcNarf__,retType__,maxArgsN__,defArgsN__)\n");
    

    #3) Definition if none of the first two are true
    f.write("#define i_RP_DECLARE_FUNC"+str(i)+"_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+")   \n");
    f.write("#define i_RP_DECLARE_FUNC"+str(i)+"_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+",");
        j+=1;
    f.write("arg__"+str(j)+")   \n");
    
    
    #end of one loop
    i-=1;

#FINAL for FUNC0
f.write("\n\n//! This macro is used to declare a function with 0 arguments in a header file\n");
#0)write the general definition which picks up the rest
f.write("#define i_RP_DECLARE_FUNC0(funcNarf__,retType__,maxArgsN__,defArgsN__)\\\n");
#now call the macro which picks the appropriate macro definition
f.write("i_RP_PICK_FUNC_DECL(i_RP_DECLARE_FUNC0_IMP,i_RP_DECL_MACRO_CHECK1(maxArgsN__,defArgsN__,0)");
f.write(",i_RP_DECL_MACRO_CHECK2(maxArgsN__,defArgsN__,0))(funcNarf__,retType__,maxArgsN__,defArgsN__)\n");
#1) Definition 1 if if compulsory number of arguments is equal to current
f.write("#define i_RP_DECLARE_FUNC"+str(i)+"_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__)  ");
#now declare the function
f.write("retType__  funcNarf__##0();\n");
#2) Definition 2,  if compulsory number of arguments is less than current (can never be less than zero
f.write("#define i_RP_DECLARE_FUNC0_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__)   \n");
#3) Definition if none of the first two are true
f.write("#define i_RP_DECLARE_FUNC0_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__)   \n");
f.write("#define i_RP_DECLARE_FUNC0_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__)   \n");


#at the end close the file
print("rf_xmacro_decl.h has been generated!");
f.close();

