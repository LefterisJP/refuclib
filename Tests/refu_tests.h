/**
** Just a simple header file to conveniently define anything that
** would be commonly needed in all tests
**/


// prevent stdout buffering for test reports.
// Must-have when debugging a test that crushes
#define rfPrintf_NOBUFF(...)	\
    do{                                         \
        rfPrintf(__VA_ARGS__);                  \
        fflush(stdout);}while(0)


#define SIGNAL_POSITION()  \
    rfPrintf("passed >> line [%d] of test file \"%s\"\n", \
             __LINE__, __FILE__)


#define COMPARISON(value1_, value2_, cmp_)         \
    do{                                                                 \
        if((value1_) cmp_ (value2_))                                    \
        {                                                               \
            rfPrintf_NOBUFF(                                            \
                "*ERROR*: A function or statement did not act as it was " \
                "expected to ==> at line [%d] of test file \"%s\"\n",   \
                __LINE__, __FILE__);                                    \
        }                                                               \
        else                                                            \
        {                                                               \
            SIGNAL_POSITION();                                          \
        }                                                               \
    }while(0)

#define COMPARISON_MSG(value1_, value2_, cmp_, msg_)                    \
    do{                                                                 \
    if((value1_) cmp_ (value2_))                                        \
    {                                                                   \
        rfPrintf_NOBUFF(                                                \
            "*ERROR*: "msg_" ==> at line [%d] of test file \"%s\"\n",   \
            __LINE__, __FILE__);                                        \
    }                                                                   \
    else                                                                \
    {                                                                   \
        SIGNAL_POSITION();                                              \
    }                                                                   \
}while(0)

// The EXPECT family of macros, used in the tests
#define EXPECT(value1_, value2_)                \
     COMPARISON(value1_, value2_, !=)

#define EXPECT_MSG(value1_, value2_, message_)      \
     COMPARISON_MSG(value1_, value2_, !=, message_)


#define EXPECTNOT(value1_, value2_)\
     COMPARISON(value1_, value2_, ==)		

#define EXPECTNOT_MSG(value1_, value2_, message_)      \
     COMPARISON_MSG(value1_, value2_, ==, message_)


#define EXPECTGE(value1_, value2_)\
     COMPARISON(value1_, value2_, <)		

#define EXPECTGE_MSG(value1_, value2_, message_)      \
     COMPARISON_MSG(value1_, value2_, <, message_)
