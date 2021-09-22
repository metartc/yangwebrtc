#ifndef INCLUDE_YANGUTIL_YANGTYPE_H_
#define INCLUDE_YANGUTIL_YANGTYPE_H_
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "YangErrorCode.h"
#include "../Yang_Config.h"


#define yang_delete(a) {if( (a)) {delete (a); (a) = NULL;}}
#define yang_deleteA(a) {if( (a)) {delete[] (a); (a) = NULL;}}
#define yang_free(a) {if( (a)) {free((a)); (a) = NULL;}}
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

#define yang_usleep usleep

#define YangAutoFree(className, instance) \
impl_YangAutoFree<className> _auto_free_##instance(&instance, false)
#define YangAutoFreeA(className, instance) \
impl_YangAutoFree<className> _auto_free_array_##instance(&instance, true)
template<class T>
class impl_YangAutoFree
{
private:
    T** ptr;
    bool is_array;
public:
    impl_YangAutoFree(T** p, bool array) {
        ptr = p;
        is_array = array;
    }

    virtual ~impl_YangAutoFree() {
        if (ptr == NULL || *ptr == NULL) {
            return;
        }

        if (is_array) {
            delete[] *ptr;
        } else {
            delete *ptr;
        }

        *ptr = NULL;
    }
};
#endif /* INCLUDE_YANGUTIL_YANGTYPE_H_ */
