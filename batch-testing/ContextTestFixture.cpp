
/// \author James Hughes
/// \date   November 2013

#include <fstream>
#include "ContextTestFixture.hpp"

#include <spire/src/FileUtil.h>
#include <spire/src/GLMathUtil.h>

namespace CPM_GL_BATCH_TESTING_NS {

//------------------------------------------------------------------------------
void ContextTestFixture::SetUp()
{
  // Build spire using the context from GlobalTestEnvironment.
  std::shared_ptr<BatchEnvironment> ctx 
      = GlobalTestEnvironment::instance()->getBatchEnvironment();
  ctx->makeCurrent();

  std::vector<std::string> shaderSearchDirs;
  shaderSearchDirs.push_back("shaders");

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cerr << "Incomplete framebuffer!" << std::endl;
    return;
  }
}

//------------------------------------------------------------------------------
void ContextTestFixture::TearDown()
{
}

//------------------------------------------------------------------------------
void ContextTestFixture::beginFrame()
{
  // Do not even attempt to render if the framebuffer is not complete.
  // This can happen when the rendering window is hidden (in SCIRun5 for
  // example);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cerr << "Incomplete framebuffer!" << std::endl;
    return;
  }

  /// \todo Move this outside of the interface!
  GL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
  GL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
}

//------------------------------------------------------------------------------
void ContextTestFixture::endFrame()
{
  std::shared_ptr<BatchEnvironment> ctx 
      = GlobalTestEnvironment::instance()->getBatchEnvironment();
  ctx->swapBuffers();
}

//------------------------------------------------------------------------------
void ContextTestFixture::compareFBOWithExistingFile(
    const std::string& filename,
    const std::string& outputDir,
    const std::string& compareDir,
    const std::string& binary,
    int pixelThreshold)
{
  // Print out the frame and compare it.
  std::string imageName = filename;

  std::string targetImage = outputDir;
  targetImage += "/" + imageName;
  GlobalTestEnvironment::instance()->getBatchEnvironment()->writeFBO(targetImage);

  // Need a better system for testing this. We should switch to boost
  // filesystem since this is only testing code, and won't be used in prod.
  {
    std::ifstream file(targetImage);
    ASSERT_FALSE(!file) << "Failed to write output image: " << targetImage;
    file.close();
  }

  // Perform the perceptual comparison using the given regression directory.
  std::string compImage = compareDir;
  compImage += "/" + imageName;

  {
    std::ifstream file(compImage);
    ASSERT_FALSE(!file) << "Failed to find comparison image: " << compImage;
    file.close();
  }

  // Test using perceptual comparison program that the user has provided
  // (hopefully).
  std::stringstream ss;
  ss << binary << " -threshold " << pixelThreshold << " " << targetImage << " " << compImage;
  std::string command = ss.str();

  // Usually the return code of std::system is implementation specific. But the
  // majority of systems end up returning the exit code of the program.
  if (std::system(command.c_str()) != 0)
  {
    // The images are NOT the same. Alert the user.
    FAIL() << "Perceptual compare of " << imageName << " failed.";
  }
}

} // namespace CPM_GL_BATCH_TESTING_NS

