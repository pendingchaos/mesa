/*
 * Mesa 3-D graphics library
 *
 * Copyright (C) 2018 PendingChaos.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */


/**
 * \file conservativeraster.c
 * glConservativeRasterParameteriNV and glConservativeRasterParameterfNV functions
 */

#include "conservativeraster.h"
#include "context.h"
#include "enums.h"

static void
conservative_raster_parameter(GLenum pname, GLdouble param,
                              bool no_error, bool integer)
{
   GET_CURRENT_CONTEXT(ctx);

   bool dilate = ctx->Extensions.NV_conservative_raster_dilate;
   bool pre_snap = ctx->Extensions.NV_conservative_raster_pre_snap_triangles;
   if (!dilate && !pre_snap) {
      _mesa_error(ctx, GL_INVALID_OPERATION, "glSubpixelPrecisionBiasNV");
      return;
   }

   if (MESA_VERBOSE & VERBOSE_API)
      _mesa_debug(ctx, "glConservativeRasterParameter%cNV(%s, %g)\n",
                  integer?'i':'f', _mesa_enum_to_string(pname), param);

   ASSERT_OUTSIDE_BEGIN_END(ctx);

   switch (pname) {
   case GL_CONSERVATIVE_RASTER_DILATE_NV:
      {
         if (!ctx->Extensions.NV_conservative_raster_dilate) {
            _mesa_error(ctx, GL_INVALID_OPERATION, "glConservativeRasterParameter%cNV",
                        integer?'i':'f');
            return;
         }

         if (param < 0.0) {
            _mesa_error(ctx, GL_INVALID_VALUE,
                        "glConservativeRasterParameter%cNV(param=%g)",
                        integer?'i':'f', param);
            return;
         }
         if (param < ctx->Const.ConservativeRasterDilateRange[0])
            param = ctx->Const.ConservativeRasterDilateRange[0];
         if (param > ctx->Const.ConservativeRasterDilateRange[1])
            param = ctx->Const.ConservativeRasterDilateRange[1];
         ctx->ConservativeRasterDilate = param;
      }
      break;
   case GL_CONSERVATIVE_RASTER_MODE_NV:
      {
         if (!ctx->Extensions.NV_conservative_raster_pre_snap_triangles) {
            _mesa_error(ctx, GL_INVALID_OPERATION, "glConservativeRasterParameter%cNV",
                        integer?'i':'f');
            return;
         }

         if (param!=GL_CONSERVATIVE_RASTER_MODE_POST_SNAP_NV &&
             param!=GL_CONSERVATIVE_RASTER_MODE_PRE_SNAP_TRIANGLES_NV) {
            _mesa_error(ctx, GL_INVALID_ENUM,
                        "glConservativeRasterParameter%cNV(param=%s)",
                        integer?'i':'f', _mesa_enum_to_string(param));
            return;
         }
         ctx->ConservativeRasterMode = param;
      }
      break;
   default:
      {
         _mesa_error(ctx, GL_INVALID_ENUM,
                     "glConservativeRasterParameter%cNV(pname=%s)",
                     integer?'i':'f', _mesa_enum_to_string(pname));
         return;
      }
      break;
   }

   FLUSH_VERTICES(ctx, 0);
   ctx->NewDriverState |=
      ctx->DriverFlags.NewNvConservativeRasterizationParams;
}

void GLAPIENTRY
_mesa_ConservativeRasterParameteriNV_no_error(GLenum pname, GLint param)
{
   conservative_raster_parameter(pname, param, true, true);
}

void GLAPIENTRY
_mesa_ConservativeRasterParameteriNV(GLenum pname, GLint param)
{
   conservative_raster_parameter(pname, param, false, true);
}

void GLAPIENTRY
_mesa_ConservativeRasterParameterfNV_no_error(GLenum pname, GLfloat param)
{
   conservative_raster_parameter(pname, param, true, false);
}

void GLAPIENTRY
_mesa_ConservativeRasterParameterfNV(GLenum pname, GLfloat param)
{
   conservative_raster_parameter(pname, param, false, false);
}

void
_mesa_init_conservative_raster(struct gl_context *ctx)
{
   ctx->ConservativeRasterDilate = 0.0;
   ctx->ConservativeRasterMode = GL_CONSERVATIVE_RASTER_MODE_POST_SNAP_NV;
}
