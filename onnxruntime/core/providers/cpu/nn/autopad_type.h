// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "core/common/common.h"
#include "core/common/exceptions.h"

namespace onnxruntime {

enum class AutoPadType {
  NOTSET = 0,
  VALID = 1,
  SAME_UPPER = 2,
  SAME_LOWER = 3,
};

inline AutoPadType StringToAutoPadType(const std::string& str) {
  if (str.empty()) {
    return AutoPadType::NOTSET;
  } else if (str == "NOTSET") {  // in onnx spec, default value is "NOTSET"
    return AutoPadType::NOTSET;
  } else if (str == "VALID") {
    return AutoPadType::VALID;
  } else if (str == "SAME_UPPER") {
    return AutoPadType::SAME_UPPER;
  } else if (str == "SAME_LOWER") {
    return AutoPadType::SAME_LOWER;
  } else {
    ORT_ENFORCE(false, "Unknown AutoPadType String");
  }
}
}  // namespace onnxruntime
