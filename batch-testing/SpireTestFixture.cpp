
/// \author James Hughes
/// \date   November 2013

#include "SpireTestFixture.hpp"

#include <spire/src/FileUtil.h>
#include <spire/src/GLMathUtil.h>
#include <gl-state/GLState.hpp>

namespace CPM_GL_BATCH_TESTING_NS {

//------------------------------------------------------------------------------
void SpireTestFixture::SetUp()
{
  // Build spire using the context from GlobalTestEnvironment.
  std::shared_ptr<BatchEnvironment> ctx 
      = GlobalTestEnvironment::instance()->getBatchEnvironment();
  ctx->makeCurrent();

  std::vector<std::string> shaderSearchDirs;
  shaderSearchDirs.push_back("shaders");

  mSpireContext = std::shared_ptr<SpireContext>(new SpireContext(ctx));
  mSpire = std::shared_ptr<CPM_SPIRE_NS::Interface>(
      new CPM_SPIRE_NS::Interface(mSpireContext, shaderSearchDirs));

  // Add default attributes.
  addDefaultAttributes();

  // Begin a new frame.
  beginFrame();
}

//------------------------------------------------------------------------------
void SpireTestFixture::TearDown()
{
  mSpire.reset();
  mSpireContext.reset();
}

//------------------------------------------------------------------------------
void SpireTestFixture::addDefaultAttributes()
{
  mSpire->addShaderAttribute("aPos",         3,  false,  sizeof(float) * 3,  CPM_SPIRE_NS::Interface::TYPE_FLOAT);
  mSpire->addShaderAttribute("aNormal",      3,  false,  sizeof(float) * 3,  CPM_SPIRE_NS::Interface::TYPE_FLOAT);
  mSpire->addShaderAttribute("aColorFloat",  4,  false,  sizeof(float) * 4,  CPM_SPIRE_NS::Interface::TYPE_FLOAT);
  mSpire->addShaderAttribute("aColor",       4,  true,   sizeof(char) * 4,   CPM_SPIRE_NS::Interface::TYPE_UBYTE);

  // Used to test attribute mis-alignment.
  mSpire->addShaderAttribute("aFieldData",   1,  false,  sizeof(float) * 1,  CPM_SPIRE_NS::Interface::TYPE_FLOAT);
}

//------------------------------------------------------------------------------
void SpireTestFixture::beginFrame()
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

  CPM_GL_STATE_NS::GLState defaultGLState;
  defaultGLState.apply();
}

//------------------------------------------------------------------------------
void SpireTestFixture::endFrame()
{
  mSpireContext->swapBuffers();
}

//------------------------------------------------------------------------------
void SpireTestFixture::compareFBOWithExistingFile(
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

  EXPECT_TRUE(CPM_SPIRE_NS::fileExists(targetImage)) << "Failed to write output image! " << targetImage;

  // Perform the perceptual comparison using the given regression directory.
  std::string compImage = compareDir;
  compImage += "/" + imageName;

  ASSERT_TRUE(CPM_SPIRE_NS::fileExists(compImage)) << "Failed to find comparison image! " << compImage;
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

//------------------------------------------------------------------------------
// SpireContext implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
SpireContext::SpireContext(std::shared_ptr<BatchEnvironment> env) :
    mEnv(env)
{ }

//------------------------------------------------------------------------------
void SpireContext::makeCurrent()
{
  mEnv->makeCurrent();
}

//------------------------------------------------------------------------------
void SpireContext::swapBuffers()
{
  mEnv->swapBuffers();
}

} // namespace CPM_GL_BATCH_TESTING_NS

