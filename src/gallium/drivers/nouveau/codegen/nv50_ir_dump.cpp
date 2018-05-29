#include "nv50_ir_dump.h"

#include "codegen/nv50_ir_target.h"
#include "tgsi/tgsi_dump.h"
#include "util/crc32.h"

#ifdef DEBUG
static char *
createDumpFilename(const char *dir, const nv50_ir_prog_info *info, const char *ext)
{
   char* fname = (char*)MALLOC(strlen(dir) + 13 + strlen(ext));
   if (dir[0] && dir[strlen(dir) - 1] == '/')
      sprintf(fname, "%s%.8x", dir, info->bin.sourceHash);
   else
      sprintf(fname, "%s/%.8x", dir, info->bin.sourceHash);

   switch (info->type) {
   case PIPE_SHADER_VERTEX:
      strcat(fname, ".vs");
      break;
   case PIPE_SHADER_TESS_CTRL:
      strcat(fname, ".tcs");
      break;
   case PIPE_SHADER_TESS_EVAL:
      strcat(fname, ".tes");
      break;
   case PIPE_SHADER_GEOMETRY:
      strcat(fname, ".gs");
      break;
   case PIPE_SHADER_FRAGMENT:
      strcat(fname, ".fs");
      break;
   case PIPE_SHADER_COMPUTE:
      strcat(fname, ".cs");
      break;
   default:
      assert(0);
      break;
   }

   strcat(fname, ext);

   return fname;
}

extern "C" {

void
nv50_ir_create_source_hash(nv50_ir_prog_info *info)
{
   switch (info->bin.sourceRep) {
   case PIPE_SHADER_IR_TGSI: {
      const tgsi_header* header = (const tgsi_header*)info->bin.source;
      unsigned size = (header->HeaderSize + header->BodySize) * sizeof(tgsi_token);
      info->bin.sourceHash = util_hash_crc32(info->bin.source, size);
      break;
   }
   default:
      assert(0);
      break;
   }
}

FILE *
nv50_ir_begin_dump(const nv50_ir_prog_info *info, const char *what,
                   const char *ext, bool binary)
{
   const char *dump_dir = debug_get_option("NV50_PROG_DUMP", NULL);
   if (!dump_dir)
      return NULL;

   char* fname = createDumpFilename(dump_dir, info, ext);

   FILE *fp = fopen(fname, binary ? "wb" : "w");
   if (!fp) {
      INFO("Failed to dump %s of a program to %s\n", what, fname);
      return NULL;
   }

   INFO("Dumping %s of a program to %s\n", what, fname);

   FREE(fname);

   return fp;
}

bool
nv50_ir_get_replacement(const nv50_ir_prog_info *info, const char *what,
                        const char *ext, size_t *size, void **data)
{
   const char *replace_dir = debug_get_option("NV50_PROG_REPLACE", NULL);
   if (!replace_dir)
      return false;

   char* fname = createDumpFilename(replace_dir, info, ext);

   FILE *fp = fopen(fname, "rb");
   if (!fp)
      return false;

   *size = 0;
   *data = MALLOC(65536);

   size_t bufSize = 65536;
   size_t read = 0;
   while ((read = fread(*data, 1, bufSize - *size, fp))) {
      *size += read;
      if (*size == bufSize) {
         *data = REALLOC(*data, bufSize, bufSize * 2);
         bufSize *= 2;
      }
   }

   INFO("Replacing code of a program with that from %s\n", fname);

   FREE(fname);

   return true;
}

}

namespace nv50_ir {

void
dumpProgramCodeAndIR(const nv50_ir::Program *prog)
{
   FILE *fp = nv50_ir_begin_dump(prog->driver, "code", ".bin", true);
   if (fp) {
      fwrite(prog->code, prog->binSize, 1, fp);
      fclose(fp);
   }

   switch (prog->driver->bin.sourceRep) {
   case PIPE_SHADER_IR_TGSI: {
      const tgsi_token *tokens = (const tgsi_token *)prog->driver->bin.source;
      fp = nv50_ir_begin_dump(prog->driver, "tgsi", ".tgsi.txt", false);
      if (fp) {
         tgsi_dump_to_file(tokens, 0, fp);
         fclose(fp);
      }
      break;
   }
   default:
      assert(0);
      break;
   }
}

bool
replaceProgramCode(nv50_ir::Program *prog)
{
   const nv50_ir::Target* targ = prog->getTarget();

   size_t size;
   void *data;
   if (!nv50_ir_get_replacement(prog->driver, "code", ".bin", &size, &data))
      return false;

   FREE(prog->code);
   prog->code = (uint32_t*)data;
   prog->binSize = size;
   prog->maxGPR = targ->getFileSize(nv50_ir::FILE_GPR) - 1;
   prog->tlsSize = targ->getFileSize(nv50_ir::FILE_MEMORY_LOCAL);

   return true;
}

} // namespace nv50_ir

#endif
