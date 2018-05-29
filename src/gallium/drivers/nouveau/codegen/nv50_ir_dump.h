#ifndef __NV50_IR_DUMP__
#define __NV50_IR_DUMP__

#include <stdio.h>
#include "util/macros.h" /* For ALWAYS_INLINE */
#include "nv50_ir_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
void
nv50_ir_create_source_hash(struct nv50_ir_prog_info *info);

FILE *
nv50_ir_begin_dump(const struct nv50_ir_prog_info *info, const char *what,
                   const char *ext, bool binary);

bool
nv50_ir_get_replacement(const struct nv50_ir_prog_info *info, const char *what,
                        const char *ext, size_t *size, void **data);
#else
ALWAYS_INLINE void
nv50_ir_create_source_hash(struct nv50_ir_prog_info *info) {
   info->bin.sourceHash = 0;
}

ALWAYS_INLINE FILE *
nv50_ir_begin_dump(const struct nv50_ir_prog_info *info, const char *what,
                   const char *ext, bool binary)
{
   return NULL;
}

ALWAYS_INLINE bool
nv50_ir_get_replacement(const struct nv50_ir_prog_info *info, const char *what,
                        const char *ext, size_t *size, void **data)
{
   return false;
}
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include "nv50_ir.h"

namespace nv50_ir {

#ifdef DEBUG
void
dumpProgramCodeAndIR(const Program *prog);

bool
replaceProgramCode(Program *prog);
#else
ALWAYS_INLINE void
dumpProgramCodeAndIR(Program *prog) {}

ALWAYS_INLINE bool
replaceProgramCode(Program *prog) {return false;}
#endif

} // namespace nv50_ir
#endif

#endif
