/// \author James Hughes
/// \date   November 2013

#include "GlobalGTestEnv.hpp"
#include "BatchEnvironment.hpp"

namespace CPM_GL_BATCH_TESTING_NS {

GlobalTestEnvironment* GlobalTestEnvironment::mInstance = nullptr;

GlobalTestEnvironment::GlobalTestEnvironment()
{
  if (mInstance != nullptr)
    throw std::runtime_error("Global test environment has already been constructed!");

  mInstance = this;
  mEnvironment = std::shared_ptr<BatchEnvironment>(
      new BatchEnvironment(600, 600, 32, 24, 8, true, false));
}

GlobalTestEnvironment* GlobalTestEnvironment::instance()
{
  if (mInstance == nullptr)
    throw std::runtime_error("Global test environment not initialized!");
  return mInstance;
}

} // namespace CPM_GL_BATCH_TESTING_NS

