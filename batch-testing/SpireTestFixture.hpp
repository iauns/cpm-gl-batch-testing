
/// \author James Hughes
/// \date   November 2013

#ifndef IAUNS_SPIREFIXTURE_HPP
#define IAUNS_SPIREFIXTURE_HPP

#include <gtest/gtest.h>
#include <spire/Interface.h>

#include "BatchEnvironment.hpp"
#include "GlobalTestEnvironment.hpp"

namespace CPM_GL_BATCH_TESTING_NS {

class SpireContext : public CPM_SPIRE_NS::Context
{
public:
  SpireContext(std::shared_ptr<BatchEnvironment> env);

  void makeCurrent() override;
  void swapBuffers() override;

private:
  std::shared_ptr<BatchEnvironment> mEnv;
};

class SpireTestFixture : public testing::Test
{
public:
  SpireTestFixture() {}

  virtual void SetUp() override;
  virtual void TearDown() override;

  void beginFrame();

  std::shared_ptr<CPM_SPIRE_NS::Interface>    mSpire;

private:
  void addDefaultAttributes();

  std::shared_ptr<SpireContext>         mSpireContext;
};

} // namespace CPM_GL_BATCH_TESTING_NS

#endif 
