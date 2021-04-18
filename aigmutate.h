#ifndef __AIGMUTATE_H__
#define __AIGMUTATE_H__
#include "types.h"
#include "aiger.h"
void mutateFrombuf(u8** buf, u32* buf_len);
void aigmutate(aiger* aig);
#endif
