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

#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "BatchEnvironment.hpp"

#include <gl-batch-context/Context.hpp>
#include <lodepng/lodepng.h>

namespace CPM_GL_BATCH_ENV_NS {

//------------------------------------------------------------------------------
BatchEnvironment::BatchEnvironment(uint32_t width, uint32_t height,
                   int32_t colorBits, int32_t depthBits, int32_t stencilBits,
                   bool doubleBuffer, bool visible) :
    mWidth(width),
    mHeight(height),
    mContext(NULL)
{
  // Pre-allocate an image of appropriate size.
  try
  {
    mRawImage.resize(width*height*4);  // 4 is the bit depth - RGBA.
  }
  catch (...)
  {
    mRawImage.clear();
  }

  if ( mRawImage.empty() )
  {
    throw std::runtime_error("Unable to allocate space for image.");
  }

  // This should be a wrapped in a shared_ptr -- RAII.
  mContext = CPM_GL_BATCH_CONTEXT_NS::Context::createBatchContext(
      width, height,
      static_cast<uint8_t>(colorBits),
      static_cast<uint8_t>(depthBits),
      static_cast<uint8_t>(stencilBits),
      doubleBuffer, visible);
  mContext->makeCurrent();

  GL(glViewport(0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height)));

  // Build an FBO (the only thing we can render to when rendering offscreen)
  /// \todo Migrate this code elsewhere when we add tuvok / a compasited
  ///       renderer as it will no longer work.
  GL(glGenFramebuffersEXT(1, &mGLFrameBuffer));
  GL(glGenRenderbuffersEXT(1, &mGLDepthBuffer));
  GL(glGenTextures(1, &mGLColorTexture));

  GL(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mGLFrameBuffer));

  GL(glBindTexture(GL_TEXTURE_2D, mGLColorTexture));
  GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(mWidth),
                  static_cast<GLsizei>(mHeight), 0,
                  GL_RGBA, GL_INT, NULL));
  GL(glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                               GL_TEXTURE_2D, mGLColorTexture, 0));


  // initialize depth renderbuffer
  GL(glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mGLDepthBuffer));
  GL(glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                              static_cast<GLsizei>(mWidth), static_cast<GLsizei>(mHeight)));
  GL(glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
                                  GL_RENDERBUFFER_EXT, mGLDepthBuffer));

  mContext->makeCurrent();
  GL(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mGLFrameBuffer));
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Frame buffer index: " << mGLFrameBuffer << std::endl;
    std::cout << "Depth buffer index: " << mGLDepthBuffer << std::endl;
    std::cout << "Color texture index: " << mGLColorTexture << std::endl;
    std::cerr << " *** Unable to generate a complete frame buffer! *** " << std::endl;
  }
}

//------------------------------------------------------------------------------
BatchEnvironment::~BatchEnvironment()
{
  if (mContext)
  {
    delete mContext;
    mContext = NULL;
  }
}

//------------------------------------------------------------------------------
void BatchEnvironment::makeCurrent()
{
  mContext->makeCurrent();
}

//------------------------------------------------------------------------------
void BatchEnvironment::swapBuffers()
{
  mContext->swapBuffers();
}

//------------------------------------------------------------------------------
int BatchEnvironment::getScreenHeight()
{
  return mHeight;
}

//------------------------------------------------------------------------------
int BatchEnvironment::getScreenWidth()
{
  return mWidth;
}

//------------------------------------------------------------------------------
void BatchEnvironment::writeFBO(const std::string& file)
{
  GL(glPixelStorei(GL_PACK_ALIGNMENT, 1));
  GL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
  GL(glReadBuffer(GL_COLOR_ATTACHMENT0));     // Reading straight from the FBO
  GL(glReadPixels(0,0,static_cast<GLsizei>(mWidth),static_cast<GLsizei>(mHeight),
                  GL_RGBA,GL_UNSIGNED_BYTE,&mRawImage[0]));

  // Flip the image before we write it out to disk. OpenGL's origin is
  // the lower left.
  size_t stride = mWidth * 4;
  uint8_t* rowCache = (uint8_t*)std::malloc(stride);
  // Either mHeight will be divisible by 2 (even) or it will be odd.
  // In the case that it is odd, mHeight % 2 == 1, and mHeight will be rounded
  // down to the nearest integer. Our flipping stops there.
  uint8_t* imgPtr = &mRawImage[0];
  for (int y = 0; y < (int)(mHeight / 2); y++)
  {
    int topY = y;
    int botY = mHeight - (y + 1);
    uint8_t* topPtr = imgPtr + topY * stride;
    uint8_t* botPtr = imgPtr + botY * stride;

    // Copy the bottom row into cache.
    std::memcpy(rowCache, botPtr, stride);

    // Replace the bottom row with the top.
    std::memcpy(botPtr, topPtr, stride);

    // Replace the top row with the cache.
    std::memcpy(topPtr, rowCache, stride);
  }

  unsigned error = lodepng::encode(file.c_str(), imgPtr, mWidth, mHeight);

  if(error)
  {
    std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    throw std::runtime_error("Libpng encoder error");
  }
}

} // namespace CPM_GL_BATCH_ENV_NS 

