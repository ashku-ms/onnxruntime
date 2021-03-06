// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "core/framework/environment.h"
#include "gtest/gtest.h"
#include "test/test_environment.h"

GTEST_API_ int main(int argc, char** argv) {
  int status = 0;

  try {
    ::onnxruntime::test::TestEnvironment test_environment{argc, argv};

    status = RUN_ALL_TESTS();
  } catch (const std::exception& ex) {
    std::cerr << ex.what();
    status = -1;
  }

  return status;
}
