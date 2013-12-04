/*
 *    == START OF REFU LICENSE ==
 *
 * Copyright (c) 2011-2013, Karapetsas Eleftherios
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the Original Author of Refu nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES;LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *    == END OF REFU LICENSE ==
*/


/*------------- Corrensponding Header inclusion -------------*/
#include <IO/rf_file.h>
/*------------- Outside Module inclusion -------------*/
#include <Utils/log.h> //for error logging
#include <Definitions/retcodes.h> //for error codes
#include <Utils/localscope.h> //for the local scope macros
/*------------- End of includes -------------*/

//Opens another process as a pipe
FILE* rf_popen(void* commandP,const char* mode)
{
    FILE* ret = 0;
    RFstring* command = (RFstring*)commandP;
    RF_ENTER_LOCAL_SCOPE();
#if RF_OPTION_DEBUG
    if( strcmp(mode,"r") != 0 && strcmp(mode,"w") != 0)
    {
        RF_ERROR("Invalid mode argument provided to rf_popen()");
        goto cleanup;
    }
#endif

    ret = _popen(command->bytes,mode);

#if RF_OPTION_DEBUG
  cleanup:
#endif
    RF_EXIT_LOCAL_SCOPE();
    return ret;
}

//Closes a pipe
int rf_pclose(FILE* stream)
{
    return _pclose(stream);
}
