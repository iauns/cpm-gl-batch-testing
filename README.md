cpm-gl-batch-testing
====================

[![Build Status](https://travis-ci.org/iauns/cpm-gl-batch-testing.png)](https://travis-ci.org/iauns/cpm-gl-batch-testing)

An CPM module holding various resources for batch testing OpenGL and Spire
based modules.

Usage
-----

### GlobalTestEnvironment

```c++
#include <batch-testing/GlobalTestEnvironment.hpp>
```

Global test environment derived from gtests's `::testing::Environment`. Sets
up `BatchEnvironment` for every one of your tests.

#### Examples

See the unit tests for https://github.com/iauns/cpm-arc-ball for an example on
how to use this class.

### BatchEnvironment

```c++
#include <batch-testing/BatchEnvironment.hpp>
```

Used to generate a batch rendering context and framebuffer. Has utility
functions to write out FBO to a PNG file. See
[documentation](http://iauns.github.io/cpm-gl-batch-testing/class_c_p_m___g_l___b_a_t_c_h___t_e_s_t_i_n_g___n_s_1_1_batch_environment.html)

### Spire Test Fixture

```c++
#include <batch-testing/SpireTestFixture.hpp>
```

GTest test fixture that initializes [Spire](https://github.com/SCIInstitute/spire). 
See [documentation](http://iauns.github.io/cpm-gl-batch-testing/class_c_p_m___g_l___b_a_t_c_h___t_e_s_t_i_n_g___n_s_1_1_spire_test_fixture.html).


