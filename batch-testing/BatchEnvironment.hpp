/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2013 Scientific Computing and Imaging Institute,
   University of Utah.


   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/

/// \author James Hughes
/// \date   November 2013

#ifndef IAUNS_GL_BATCH_ENVIRONMENT_HPP
#define IAUNS_GL_BATCH_ENVIRONMENT_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

#include "gl-platform/GLPlatform.hpp"

namespace CPM_GL_BATCH_CONTEXT_NS {
class Context;
}

namespace CPM_GL_BATCH_TESTING_NS {

/// OpenGL batch rendering environment.
class BatchEnvironment
{
public:
  BatchEnvironment(uint32_t width, uint32_t height,
                   int32_t colorBits, int32_t depthBits, int32_t stencilBits,
                   bool doubleBuffer, bool visible);
  virtual ~BatchEnvironment();
  
  /// Writes a PNG file of the current FBO state.
  void writeFBO(const std::string& file);

  int getScreenWidth();
  int getScreenHeight();

  // We really want an interface definition here (really want BatchEnvironment's
  // type to conform to some idea of what a context is).
  void makeCurrent();
  void swapBuffers();

private:

  int mWidth;
  int mHeight;

  /// We aren't supporting c++11, so we don't use a shared ptr here.
  std::shared_ptr<CPM_GL_BATCH_CONTEXT_NS::Context> mContext;

  /// Pre-allocated image data. The memory we use to pull image data
  /// from OpenGL.
  std::vector<uint8_t> mRawImage;

  GLuint mGLFrameBuffer;
  GLuint mGLColorTexture;
  GLuint mGLDepthBuffer;
};

} // namespace CPM_GL_BATCH_ENV_NS 

#endif 
