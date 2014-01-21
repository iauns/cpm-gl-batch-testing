
/// \author James Hughes
/// \date   November 2013

#ifndef CPM_GL_BATCH_TESTING_CONTEXT_TEST_FIXTURE_HPP
#define CPM_GL_BATCH_TESTING_CONTEXT_TEST_FIXTURE_HPP

#include <memory>
#include <gtest/gtest.h>

#include "BatchEnvironment.hpp"
#include "GlobalGTestEnv.hpp"

namespace CPM_GL_BATCH_TESTING_NS {

class ContextTestFixture: public testing::Test
{
public:
  ContextTestFixture() {}

  virtual void SetUp() override;
  virtual void TearDown() override;

  /// Optional function. When called, the depth and color buffer are cleared. 
  void beginFrame();

  /// Swaps the buffers, if any.
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

private:
  void addDefaultAttributes();
};

} // namespace CPM_GL_BATCH_TESTING_NS

#endif 
