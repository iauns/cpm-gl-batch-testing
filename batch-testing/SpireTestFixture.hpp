
/// \author James Hughes
/// \date   November 2013

#ifndef IAUNS_SPIREFIXTURE_HPP
#define IAUNS_SPIREFIXTURE_HPP

#include <memory>
#include <gtest/gtest.h>
#include <spire/Interface.h>

#include "BatchEnvironment.hpp"
#include "GlobalGTestEnv.hpp"

namespace CPM_GL_BATCH_TESTING_NS {

class SpireContext;

class SpireTestFixture : public testing::Test
{
public:
  SpireTestFixture() {}

  virtual void SetUp() override;
  virtual void TearDown() override;

  void beginFrame();
  void endFrame();

  /// Compares the active frame buffer's contents with that given in
  /// 'filename'. This function expects that 'binary' points to the
  /// perceptual diff utility. Also, this function uses gtest macros, so it
  /// expects that it is inside of a gtest TEST.
  /// \param filename   Filename to compare against. Will search for the file
  ///                   in compareDir.
  /// \param outputDir  Directory in which to place output images.
  /// \param compareDir Directory that houses the images to compare against.
  /// \param binary     Perceputal diff binary location.
  /// \prama pixelThreshold  Number of pixels that are allowed to be different.
  void compareFBOWithExistingFile(
    const std::string& filename,
    const std::string& outputDir,
    const std::string& compareDir,
    const std::string& binary,
    int pixelThreshold);

  std::shared_ptr<CPM_SPIRE_NS::Interface>    mSpire;

private:
  void addDefaultAttributes();

  std::shared_ptr<SpireContext>         mSpireContext;
};

// Helper class deriving from spire's context. Spire's context is being used
// as an interface in this regard.
class SpireContext : public CPM_SPIRE_NS::Context
{
public:
  SpireContext(std::shared_ptr<BatchEnvironment> env);

  void makeCurrent() override;
  void swapBuffers() override;

private:
  std::shared_ptr<BatchEnvironment> mEnv;
};

} // namespace CPM_GL_BATCH_TESTING_NS

#endif 
