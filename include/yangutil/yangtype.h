#ifndef INCLUDE_YANGUTIL_YANGTYPE_H_
#define INCLUDE_YANGUTIL_YANGTYPE_H_

#include "YangErrorCode.h"
#include "../Yang_Config.h"
#include <stdint.h>
#include <yangutil/sys/YangAutoFree.h>
#define yang_delete(a) {if( (a) != NULL ) {delete (a); (a) = NULL;}}
#define yang_deleteA(a) {if( (a) != NULL ) {delete[] (a); (a) = NULL;}}
#define yang_free(a) {if( (a) != NULL ) {free((a)); (a) = NULL;}}
#define yang_min(a, b) (((a) < (b))? (a) : (b))
#define yang_max(a, b) (((a) < (b))? (b) : (a))
#define yang_freep(p) \
    if (p) { \
        delete p; \
        p = NULL; \
    } \
    (void)0
// Please use the freepa(T[]) to free an array, otherwise the behavior is undefined.
#define yang_freepa(pa) \
    if (pa) { \
        delete[] pa; \
        pa = NULL; \
    } \
    (void)0
#define Yang_Rtmp 0
#define Yang_Srt 1
#define Yang_Webrtc 2



#endif /* INCLUDE_YANGUTIL_YANGTYPE_H_ */
