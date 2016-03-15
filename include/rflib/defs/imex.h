/**
 * @author: Lefteris Karapetsas
 * @licence: BSD3 (Check repository root for details)
 */
#ifndef RF_IMEX_H
#define RF_IMEX_H

#ifdef REFU_WIN32_VERSION //windows case is the only one that needs special decl
    #ifdef REFU_DYNAMIC_LIB //export  case
        #define i_DECLIMEX_  __declspec(dllexport)
    #else//in import case or static library
        #if !( defined(REFU_STATIC_LIB) || defined(REFU_TEST)) //if this is not a static library
            #define i_DECLIMEX_  __declspec(dllimport)
        #else
			#define i_DECLIMEX_  //static means this should be empty
		#endif
    #endif
#else //in unix case or when testing
    #define i_DECLIMEX_
#endif

#endif //include guards end
