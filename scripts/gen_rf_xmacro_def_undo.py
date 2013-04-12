#the maximum number of arguments for a function
maxArgs = 36



#stuff to import to be able to open files in the parent directory
import os.path
import sys

print("Generating rf_xmacro_def_undo.h ...");
f = open(os.path.dirname(sys.argv[0]) + "/../include/Preprocessor/rf_xmacro_def_undo.h","w");

#put some required definitions at the top
f.write("/**\n** @author Lefteris\n** @date 13/02/2012\n**\n\
** This file contains #undef so to easily undefine all the macros defined by\n\
** rf_xmacro_def.h . It is automatically generated\n\
** by the python script gen_rf_xmacro_def_undo.py\n");
f.write("*/");

#Write the macros used to pick the correct macro for function definition

f.write("\n\n\n///These macros here are used in order to pickup the correct function macro definition\n")
f.write("#undef i_RP_CHECK2\n")
f.write("#undef i_RP_CHECK1\n")
f.write("#undef i_RP_MACRO_CHECK\n");
f.write("#undef i_RP_MACRO_CHECK2\n");
f.write("#undef i_RP_PICK_FUNC_DEF\n");


#Write the i_REVERSE macros to reverse the arguments list
i = maxArgs
f.write("\n///These macros are used to reverse a list of arguments. They are used to obtain the appropriate default arguments\n")
while(i >= 0):
    f.write("#undef i_REVERSE"+str(i)+"\n");
    i-=1;
 
#Now Write the macros that take in the N default arguments from the default arguments list
i = maxArgs;
f.write("\n//!These macros are used to get the appropriate number of default arguments\n\n\n")
while(i>=0):
    f.write("//! Macros to get the appropriate number of arguments for "+str(i)+"\n");
    #1)The i_AFTER_FIRSTXX MAcro
    f.write("#undef i_AFTER_FIRST"+str(i)+"\n");

    #2)The i_AFTER_FIRSTXX_NOCOMMA MAcro
    f.write("#undef i_AFTER_FIRST"+str(i)+"_NOCOMMA\n");

    #3)The i_FIRSTXX Macro
    f.write("#undef i_FIRST"+str(i)+"\n");

    #4)The i_FIRSTXX_NOCOMMA Macro
    f.write("#undef i_FIRST"+str(i)+"_NOCOMMA\n");

    
    #5)The i_LASTXX Macro
    f.write("#undef i_LAST"+str(i)+"\n");
    
    #6)THE i_LASTXX_IMP Macro
    f.write("#undef i_LAST"+str(i)+"_IMP \n ");

    #7)The i_LASTXX_NOCOMMA Macro
    f.write("#undef i_LAST"+str(i)+"_NOCOMMA \n  ");

    #8)THE i_LASTXX_NOCCOMA_IMP Macro
    f.write("#undef i_LAST"+str(i)+"_NOCCOMA_IMP \n ");

    #9)The i_RP_DEFAULT_ARGS_X Macro when last call == 0
    f.write("#undef i_RP_DEFAULT_ARGS_"+str(i)+"_LAST0\n");

    #10) The i_RP_DEFAULT_ARGS_X Macro when last call == 1
    f.write("#undef i_RP_DEFAULT_ARGS_"+str(i)+"_LAST1   \n");

    i-=1





#Add the macro that gives us the appropriate default argument macro
f.write("//! This macro gets the appropriate default arguments macro\n");
f.write("#undef i_RP_GET_DEFAULT_ARG  \n");
f.write("#undef i_RP_GET_DEFAULT_ARG_IMP \n");

###############################################################################################################
#Now Write the big bulk of the macros. Namely those which are used to define functions with default arguments
####################################################################################################################
i = maxArgs;
f.write("\n//! These macros are used when you want to define a function in a source file with default arguments and want to avoid lots of typing\n");
while(i>=0):
    f.write("//! Function definition macros for "+str(i)+" arguments functions\n");
    #0)write the macro from which everything is called
    f.write("#undef RF_DEFINE_DEFAULT_ARG_FUNC"+str(i)+" \n");


    #1)write the general definition macro which picks up the rest
    f.write("#undef i_RP_DEFINE_FUNC"+str(i)+" \n");

    #1) Definition 1 , if compulsory number of arguments is equal to current
    f.write("#undef i_RP_DEFINE_FUNC"+str(i)+"_IMP00 \n");
 

    
    #2) Definition 2, if compulsory number of arguments is less than current
    f.write("#undef i_RP_DEFINE_FUNC"+str(i)+"_IMP01  \n");
  
    #3) Definitions if none of the first two are true
    f.write("#define i_RP_DEFINE_FUNC"+str(i)+"_IMP10  \n");

    #end of one loop
    i-=1;




#at the end close the file
print("rf_xmacro_def_undo.h has been generated!");
f.close();

