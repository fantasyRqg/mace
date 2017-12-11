//
// Copyright (c) 2017 XiaoMi All rights reserved.
//

#include "mace/kernels/relu.h"
#include "mace/core/runtime/opencl/cl2_header.h"
#include "mace/core/runtime/opencl/opencl_runtime.h"
#include "mace/kernels/opencl/helper.h"
#include "mace/utils/utils.h"
#include "mace/utils/tuner.h"

namespace mace {
namespace kernels {

template <typename T>
void ReluFunctor<DeviceType::OPENCL, T>::operator()(const Tensor *input,
                                                        Tensor *output) {

  const index_t batch = input->dim(0);
  const index_t height = input->dim(1);
  const index_t width = input->dim(2);
  const index_t channels = input->dim(3);

  const index_t channel_blocks = RoundUpDiv4(channels);

  auto runtime = OpenCLRuntime::Get();
  auto program = runtime->program();

  std::set<std::string> built_options;
  auto dt = DataTypeToEnum<T>::value;
  built_options.emplace("-DDATA_TYPE=" + DtToUpstreamCLDt(dt));
  built_options.emplace("-DCMD_DATA_TYPE=" + DtToUpstreamCLCMDDt(dt));
  cl::Kernel relu_kernel;
  if (max_limit_ < 0) {
    relu_kernel  = runtime->BuildKernel("relu", "relu", built_options);

    uint32_t idx = 0;
    relu_kernel.setArg(idx++, *(static_cast<const cl::Buffer *>(input->buffer())));
    relu_kernel.setArg(idx++, *(static_cast<cl::Buffer *>(output->buffer())));
  } else {
    relu_kernel  = runtime->BuildKernel("relu", "relux", built_options);

    uint32_t idx = 0;
    relu_kernel.setArg(idx++, *(static_cast<const cl::Buffer *>(input->buffer())));
    relu_kernel.setArg(idx++, max_limit_);
    relu_kernel.setArg(idx++, *(static_cast<cl::Buffer *>(output->buffer())));
  }
  const uint32_t gws[3] = {static_cast<uint32_t>(channel_blocks),
                           static_cast<uint32_t>(width),
                           static_cast<uint32_t>(height * batch)};
  const std::vector<uint32_t> lws = {8, 16, 8};
  const uint32_t kwg_size = runtime->GetKernelMaxWorkGroupSize(relu_kernel);
  auto params_generator = [&kwg_size]() -> std::vector<std::vector<uint32_t>> {
    return {{4, 15, 8}, //SNPE size
            {kwg_size / 16, 4, 4},
            {kwg_size / 32, 4, 8},
            {kwg_size / 32, 8, 4},
            {kwg_size / 64, 8, 8},
            {kwg_size / 64, 16, 4},
            {kwg_size / 128, 8, 16},
            {kwg_size / 128, 16, 8},
            {kwg_size / 128, 32, 4},
            {1, kwg_size / 32, 32},
            {1, kwg_size / 64, 64},
            {1, kwg_size / 128, 128},
            {3, 15, 9},
            {7, 15, 9},
            {9, 7, 15},
            {15, 7, 9},
            {1, kwg_size, 1}};
  };
  auto func = [&](const std::vector<uint32_t> &params) -> cl_int {
    cl_int error = runtime->command_queue().enqueueNDRangeKernel(
        relu_kernel, cl::NullRange,
        cl::NDRange(gws[0], gws[1], gws[2]),
        cl::NDRange(params[0], params[1], params[2]),
        NULL, OpenCLRuntime::Get()->GetDefaultEvent());

    MACE_CHECK(error == CL_SUCCESS) << "Error code: " << error;
    return error;
  };
  std::stringstream ss;
  ss << "relu_opencl_kernel_"
     << output->dim(0) << "_"
     << output->dim(1) << "_"
     << output->dim(2) << "_"
     << output->dim(3);
  Tuner<uint32_t>::Get()->template TuneOrRun<cl_int>(ss.str(),
                                                     lws,
                                                     params_generator,
                                                     func);
}

template
struct ReluFunctor<DeviceType::OPENCL, float>;
template
struct ReluFunctor<DeviceType::OPENCL, half>;
}  // namespace kernels
}  // namespace mace