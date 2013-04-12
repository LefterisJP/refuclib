#the maximum number of arguments for a function
maxArgs = 36



#stuff to import to be able to open files in the parent directory
import os.path
import sys

print("Generating rf_xmacro_def.h ...");
f = open(os.path.dirname(sys.argv[0]) + "/../include/Preprocessor/rf_xmacro_def.h","w");

#put some required definitions at the top
f.write("/**\n** @file rf_xmacro_def.h\n** @author Lefteris\n** @date 13/02/2012\n**\n\
** This file contains macros to easily define functions with default arguments\n\
** in source files using the C preprocessor. It is automatically generated\n\
** by the python script gen_rf_xmacro_def.py\n");
f.write("*/");

f.write("\n#include \"rf_xmacro_sub.h\"//to be able to do subtraction with the preprocessor\n")
f.write("#include \"rf_xmacro_gt.h\"//to be able to do greater than comparisons with the preprocessor\n")
f.write("#include \"rf_xmacro_lt.h\"//to be able to do less than comparisons with the preprocessor\n\n")
f.write("#include \"rf_xmacro_eq.h\"//to be able to do equality comparisons with the preprocessor\n\n")
f.write("#include \"rf_xmacro_argcount.h\"//to be able to count the number of arguments\n\n")
f.write("#include \"rf_xmacro_utils.h\"//to use util macros for token pasting");


#Write the macros used to pick the correct macro for function definition

f.write("\n\n\n///These macros here are used in order to pickup the correct function macro definition\n")
f.write("#define i_RP_DEF_CHECK2(A__,B__)   RP_GT(B__,A__)\n")
f.write("#define i_RP_DEF_CHECK1(A__,B__)   RP_LT(B__,A__)\n")
f.write("#define i_RP_DEF_MACRO_CHECK1(maxArgsN__,defArgsN__,thisArgN__)  i_RP_DEF_CHECK1(RP_SUB(maxArgsN__,defArgsN__),thisArgN__)\n");
f.write("#define i_RP_DEF_MACRO_CHECK2(maxArgsN__,defArgsN__,thisArgN__)  i_RP_DEF_CHECK2(RP_SUB(maxArgsN__,defArgsN__),thisArgN__)\n");
f.write("#define i_RP_PICK_FUNC_DEF(funcmacro__,eqresult__,cmpresult__)  i_RP_PASTE3(funcmacro__,eqresult__,cmpresult__)\n\n");

#Write the i_REVERSE macros to reverse the arguments list
i = maxArgs
f.write("\n///These macros are used to reverse a list of arguments. They are used to obtain the appropriate default arguments\n")
while(i > 0):
    f.write("#define i_REVERSE"+str(i)+"(");
    j = 1;
    #put the parameters of the macro
    while(j<=i):
        f.write("def"+str(j)+",");
        j+=1;
    f.write("...)   ");
    #now comes the macro definition
    j=i;
    while(j>0):
        f.write("def"+str(j)+",");
        j-=1;
    f.write("\n");
    i-=1;
#final macro
f.write("#define i_REVERSE0(...)     \n");

#keep a commented example of how things were previously using REVERSE MACRO for the RP_DEFAULT_ARGS_X Macro
f.write("\n///The following is commented because I used to have it like that to reverse the arguments list. Keeping an example in case I want to remember how to do it in the future\n");
f.write("/*\n#define i_FIRST2_IMP(_1_,_2_,...)   ,_1_,_2_\n");
f.write("#define i_FIRST2(...)   i_FIRST2_IMP(__VA_ARGS__)\n");
f.write("#define i_RP_DEFAULT_ARGS_2(defArgsN__,...)   i_FIRST2(i_RP_PASTE2(i_REVERSE,defArgsN__)(__VA_ARGS__))\n*/\n");
        
#Now Write the macros that take in the N default arguments from the default arguments list
i = maxArgs;
f.write("\n//!These macros are used to get the appropriate number of default arguments\n\n\n")
while(i>0):
    f.write("//! Macros to get the appropriate number of arguments for "+str(i)+"\n");
    #1)The i_AFTER_FIRSTXX MAcro
    f.write("#define i_AFTER_FIRST"+str(i)+"(argsN__,");
    j=1;
    #put the parameters of the macro
    while(j<=i):
        f.write("p"+str(j)+"__,");
        j+=1;
    f.write("...)   ");
    #now comes the macro definition
    f.write("i_FIRST##argsN__(__VA_ARGS__) \n");

    #2)The i_AFTER_FIRSTXX_NOCOMMA MAcro
    f.write("#define i_AFTER_FIRST"+str(i)+"_NOCOMMA(argsN__,");
    j=1;
    #put the parameters of the macro
    while(j<=i):
        f.write("p"+str(j)+"__,");
        j+=1;
    f.write("...)   ");
    #now comes the macro definition
    f.write("i_FIRST##argsN__##_NOCOMMA(__VA_ARGS__) \n");

    #3)The i_FIRSTXX Macro
    f.write("#define i_FIRST"+str(i)+"(");
    #put the parameters of the macro
    j=1
    while(j<=i):
        f.write("p"+str(j)+"__,");
        j+=1;
    f.write("...)  ");
    #and now the definition
    j=1;
    while(j<=i):
        f.write(",p"+str(j)+"__");
        j+=1;
    f.write("\n");

    #4)The i_FIRSTXX_NOCOMMA Macro
    f.write("#define i_FIRST"+str(i)+"_NOCOMMA(");
    j=1
    while(j<=i):
        f.write("p"+str(j)+"__,");
        j+=1;
    f.write("...)  ");
    #and now the definition
    j=1;
    while(j<i):
        f.write("p"+str(j)+"__,");
        j+=1;
    f.write("p"+str(j)+"__ \n");
    
    #5)The i_LASTXX Macro
    f.write("#define i_LAST"+str(i)+"(defArgsN__,...)   ");
    f.write("i_LAST"+str(i)+"_IMP(i_AFTER_FIRST,RP_SUB(defArgsN__,"+str(i)+"))("+str(i)+",__VA_ARGS__)\n");
    #6)THE i_LASTXX_IMP Macro
    f.write("#define i_LAST"+str(i)+"_IMP(id__,subres__)    ");
    f.write("i_RP_PASTE2(id__,subres__)  \n");
    #7)The i_LASTXX_NOCOMMA Macro
    f.write("#define i_LAST"+str(i)+"_NOCOMMA(defArgsN__,...)   ");
    f.write("i_LAST"+str(i)+"_NOCCOMA_IMP(i_AFTER_FIRST,RP_SUB(defArgsN__,"+str(i)+"))("+str(i)+",__VA_ARGS__)\n");
    #8)THE i_LASTXX_NOCCOMA_IMP Macro
    f.write("#define i_LAST"+str(i)+"_NOCCOMA_IMP(id__,subres__)    ");
    f.write("i_RP_PASTE3(id__,subres__,_NOCOMMA)  \n");
    #9)The i_RP_DEFAULT_ARGS_X Macro when last call == 0
    f.write("#define i_RP_DEFAULT_ARGS_"+str(i)+"_LAST0(defArgsN__,...)   ");
    f.write("i_LAST"+str(i)+"(defArgsN__,__VA_ARGS__)\n");
    #was:    f.write("i_LAST"+str(i)+"(i_RP_PASTE2(i_REVERSE,defArgsN__)(__VA_ARGS__))\n");

    #10) The i_RP_DEFAULT_ARGS_X Macro when last call == 1
    f.write("#define i_RP_DEFAULT_ARGS_"+str(i)+"_LAST1(defArgsN__,...)   ");
    f.write("i_LAST"+str(i)+"_NOCOMMA(defArgsN__,__VA_ARGS__)\n\n");
    i-=1


#final case macros (i == 0)
f.write("//! Macros to get the appropriate number of arguments for 0\n");
#1)The i_AFTER_FIRSTXX_IMP MAcro
f.write("#define i_AFTER_FIRST0_IMP(argsN__,...)   ,__VA_ARGS__  \n");
#2)The i_AFTER_FIRSTXX_NOCOMMA_IMP MAcro
f.write("#define i_AFTER_FIRST0_NOCOMMA_IMP(argsN__,...)   __VA_ARGS__  \n");
#3)The i_AFTER_FIRSTXX Macro
f.write("#define i_AFTER_FIRST0(...)   ");
f.write("i_AFTER_FIRST0_IMP(__VA_ARGS__)\n");
#4)The i_AFTER_FIRSTXX_NOCOMMA Macro
f.write("#define i_AFTER_FIRST0_NOCOMMA(...)   ");
f.write("i_AFTER_FIRST0_NOCOMMA_IMP(__VA_ARGS__)\n");
#5)The i_LASTXX Macro
f.write("#define i_LAST0(defArgsN__,...)     \n");
#6)THE i_LASTXX_IMP Macro
f.write("#define i_LAST0_IMP(id__,subres__)    \n");
#7)The i_LASTXX_NOCOMMA Macro
f.write("#define i_LAST0_NOCOMMA(defArgsN__,...)   \n");
#8)THE i_LASTXX_NOCCOMA_IMP Macro
f.write("#define i_LAST0_NOCCOMA_IMP(id__,subres__)    \n");
#9)The i_RP_DEFAULT_ARGS_X Macro when last call == 0
f.write("#define i_RP_DEFAULT_ARGS_0_LAST0(defArgsN__,...)   ");
f.write("i_LAST0(defArgsN__,__VA_ARGS__)\n");
#10) The i_RP_DEFAULT_ARGS_X Macro when last call == 1
f.write("#define i_RP_DEFAULT_ARGS_0_LAST1(defArgsN__,...)   ");
f.write("i_LAST0_NOCOMMA(defArgsN__,__VA_ARGS__)\n\n");
i-=1

#Add the macro that gives us the appropriate default argument macro
f.write("//! This macro gets the appropriate default arguments macro\n");
f.write("#define i_RP_GET_DEFAULT_ARG(args__,maxArgsN__) i_RP_GET_DEFAULT_ARG_IMP(i_RP_DEFAULT_ARGS_,args__,_LAST,RP_EQ(args__,maxArgsN__))\n");
f.write("#define i_RP_GET_DEFAULT_ARG_IMP(macronarf__,args__,identifier__,last__)    i_RP_PASTE4(macronarf__,args__,identifier__,last__)\n\n");

###############################################################################################################
#Now Write the big bulk of the macros. Namely those which are used to define functions with default arguments
####################################################################################################################
i = maxArgs;
f.write("\n//! These macros are used when you want to define a function in a source file with default arguments and want to avoid lots of typing\n");
while(i>0):
    f.write("//! Function definition macros for "+str(i)+" arguments functions. No comments here. All comments are in the generating python file.\n");
    #0)write the macro from which everything is called DEFINE [RF_DEFINE_DFUNCXX]
    f.write("#define RF_DEFINE_DFUNC"+str(i)+"(retType__,funcNarf__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<=i):
        f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
        j+=1;
    f.write("...)\\\n");
    #now call the picking macro CALL [i_RP_DEFINE_FUNC]
    f.write("i_RP_DEFINE_FUNC"+str(i-1)+"(funcNarf__,retType__,"+str(i)+",RF_NARG(__VA_ARGS__),");
    #put the rest of the parameters of the macro
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
        j+=1;
    f.write("__VA_ARGS__)\\\n");
    #Finally add the definition for the full function. THIS IS WHERE THE MACRO WILL END UP. THIS IS WHAT PRECEDES USER CODE. Because of this the user can open brackets and write the function's body
    f.write("retType__ funcNarf__##"+str(i)+"(");
    #put the rest of the parameters of the macro
    j=1;
    while(j< i):
        f.write("arg__"+str(j)+"Type arg__"+str(j)+"Name,");
        j+=1;
    f.write("arg__"+str(j)+"Type arg__"+str(j)+"Name)\n\n\n");
    

    #1)write the general definition macro which picks up the rest DEFINE [i_RP_DEFINE_FUNC]
    f.write("#define i_RP_DEFINE_FUNC"+str(i)+"(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<=i):
        f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
        j+=1;
    f.write("...)\\\n");
    #now call the macro which picks the appropriate macro definition. CALL [i_RP_PICK_FUNC_DEF]
    f.write("i_RP_PICK_FUNC_DEF(i_RP_DEFINE_FUNC"+str(i)+"_IMP,i_RP_DEF_MACRO_CHECK1(maxArgsN__,defArgsN__,"+str(i)+")");
    f.write(",i_RP_DEF_MACRO_CHECK2(maxArgsN__,defArgsN__,"+str(i)+"))(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<=i):
        f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
        j+=1;
    f.write("__VA_ARGS__)\n\n");
    #1) Definition 1 , if compulsory number of arguments is equal to current DEFINE [i_RP_DEFINE_FUNCXX_IMP00]
    f.write("#define i_RP_DEFINE_FUNC"+str(i)+"_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<=i):
        f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
        j+=1;
    f.write("...)\\\n");
    #now define the function signature. Notice it is defined as inline
    f.write("inline retType__  funcNarf__##"+str(i)+"(");
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+"Type arg__"+str(j)+"Name,");
        j+=1;
    f.write("arg__"+str(j)+"Type arg__"+str(j)+"Name)\\\n");
    f.write("{\\\n");
    #now call the maxArgs function from here
    f.write("\t return  funcNarf__##maxArgsN__(");
    #the parameters of the function
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+"Name,");
        j+=1;
    f.write("arg__"+str(j)+"Name ");
    #now use the macro to get the appropiate default arguments
    f.write("i_RP_GET_DEFAULT_ARG(RP_SUB(maxArgsN__,"+str(i)+"),maxArgsN__)(defArgsN__,__VA_ARGS__));\\\n");
    f.write("}\n\n");
    
    #2) Definition 2, if compulsory number of arguments is less than current
    f.write("#define i_RP_DEFINE_FUNC"+str(i)+"_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<=i):
        f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
        j+=1;
    f.write("...)\\\n");
    #now define the function signature. Notice it is defined as inline
    f.write("inline retType__  funcNarf__##"+str(i)+"(");
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+"Type arg__"+str(j)+"Name,");
        j+=1;
    f.write("arg__"+str(j)+"Type arg__"+str(j)+"Name)\\\n");
    f.write("{\\\n");
    #now call the maxArgs function from here
    f.write("\t return funcNarf__##maxArgsN__(");
    #the parameters of the function
    j=1;
    while(j<i):
        f.write("arg__"+str(j)+"Name,");
        j+=1;
    f.write("arg__"+str(j)+"Name ");
    #now use the macro to get the appropiate default arguments
    f.write("i_RP_GET_DEFAULT_ARG(RP_SUB(maxArgsN__,"+str(i)+"),maxArgsN__)(defArgsN__,__VA_ARGS__));\\\n");
    f.write("}\\\n");
    #also call the definition macro for lower arguments number
    if(i!=1):
        f.write("i_RP_DEFINE_FUNC"+str(i-1)+"(funcNarf__,retType__,maxArgsN__,defArgsN__,");
        j=1;
        while(j<i):
            f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
            j+=1;
        f.write("__VA_ARGS__)\n");
    else:
        f.write("i_RP_DEFINE_FUNC"+str(i-1)+"(funcNarf__,retType__,maxArgsN__,defArgsN__,__VA_ARGS__)\n");


    #3) Definitions if none of the first two are true
    f.write("#define i_RP_DEFINE_FUNC"+str(i)+"_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<=i):
        f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
        j+=1;
    f.write("...)    \n");
    f.write("#define i_RP_DEFINE_FUNC"+str(i)+"_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,");
    #put the rest of the parameters of the macro
    j=1;
    while(j<=i):
        f.write("arg__"+str(j)+"Type,arg__"+str(j)+"Name,");
        j+=1;
    f.write("...)    \n");
    f.write("\n\n\n");

    
    
    #end of one loop
    i-=1;


#Final case FUNC0
f.write("//! Function definition macros for 0 arguments functions\n");
#0)write the general definition which picks up the rest
f.write("#define i_RP_DEFINE_FUNC0(funcNarf__,retType__,maxArgsN__,defArgsN__,...)\\\n");
#now call the macro which picks the appropriate macro definition
f.write("i_RP_PICK_FUNC_DEF(i_RP_DEFINE_FUNC0_IMP,i_RP_DEF_MACRO_CHECK1(maxArgsN__,defArgsN__,0)");
f.write(",i_RP_DEF_MACRO_CHECK2(maxArgsN__,defArgsN__,0))(funcNarf__,retType__,maxArgsN__,defArgsN__,");
f.write("__VA_ARGS__)\n\n");
#1) Definition 1 , if compulsory number of arguments is equal to current (only one possible)
f.write("#define i_RP_DEFINE_FUNC0_IMP00(funcNarf__,retType__,maxArgsN__,defArgsN__,...)\\\n");
#now define the function signature
f.write("retType__  funcNarf__##0()\\\n");
f.write("{\\\n");
#now call the maxArgs function from here with all the default values since this can only have default values (0 argument case)
f.write("\t funcNarf__##maxArgsN__(__VA_ARGS__);\\\n");
f.write("}\n");
#2) Definition 2, if compulsory number of arguments is less than current (illegal to happen)
f.write("#define i_RP_DEFINE_FUNC0_IMP01(funcNarf__,retType__,maxArgsN__,defArgsN__,...)   \n");
#3) Definitions if none of the first two are true
f.write("#define i_RP_DEFINE_FUNC0_IMP10(funcNarf__,retType__,maxArgsN__,defArgsN__,...)   \n");
f.write("#define i_RP_DEFINE_FUNC0_IMP11(funcNarf__,retType__,maxArgsN__,defArgsN__,...)   \n");



#at the end close the file
print("rf_xmacro_def.h has been generated!");
f.close();

