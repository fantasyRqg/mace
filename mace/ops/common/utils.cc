// Copyright 2020 The MACE Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "mace/ops/common/utils.h"

#include "mace/core/tensor.h"

namespace mace {
namespace ops {
namespace common {
namespace utils {

void GetSizeParamFromTensor(const Tensor *size_tensor, index_t *out_height,
                            index_t *out_width) {
  const int *size = size_tensor->data<int>();
  *out_height = size[0];
  *out_width = size[1];
}

}  // namespace utils
}  // namespace common
}  // namespace ops
}  // namespace mace
