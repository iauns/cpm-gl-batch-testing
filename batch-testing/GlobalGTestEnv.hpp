/// \author James Hughes
/// \date   November 2013

#ifndef GLOBAL_GTEST_ENVIRONMENT_HPP
#define GLOBAL_GTEST_ENVIRONMENT_HPP

#include <memory>
#include <gtest/gtest.h>
#include "BatchEnvironment.hpp"

namespace CPM_GL_BATCH_TESTING_NS {

class GlobalTestEnvironment : public ::testing::Environment
{
public:
  GlobalTestEnvironment();
  static GlobalTestEnvironment* instance();
  std::shared_ptr<BatchEnvironment> getBatchEnvironment() const {return mEnvironment;}

protected:
  std::shared_ptr<BatchEnvironment> mEnvironment;
  static GlobalTestEnvironment* mInstance;
};

} // CPM_GL_BATCH_TESTING_NS

#endif 

