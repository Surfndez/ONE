/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ONERT_BACKEND_GPU_CL_OPERAND_I_CL_TENSOR_H__
#define __ONERT_BACKEND_GPU_CL_OPERAND_I_CL_TENSOR_H__

#include <backend/ITensor.h>

#include "tensorflow/lite/delegates/gpu/api.h"
#include "tensorflow/lite/delegates/gpu/spi.h"
#include "tensorflow/lite/delegates/gpu/cl/cl_command_queue.h"
#include "tensorflow/lite/delegates/gpu/cl/kernels/converter.h"
#include "tensorflow/lite/delegates/gpu/cl/tensor.h"
#include "tensorflow/lite/delegates/gpu/cl/environment.h"

#include "TensorBuilderHelper.h"

namespace onert
{
namespace backend
{
namespace gpu_cl
{
namespace operand
{

class ICLTensor : public ITensor
{
public:
  ICLTensor() = default;
  ICLTensor(const ICLTensor &) = delete;
  ICLTensor &operator=(const ICLTensor &) = delete;
  ICLTensor(ICLTensor &&) = default;
  ICLTensor &operator=(ICLTensor &&) = default;

  ICLTensor(size_t rank, ir::Shape shape, std::shared_ptr<tflite::gpu::cl::Environment> environment,
            TensorType type)
    : _rank{rank}, _shape{shape}, _environment(environment), _type(type)
  {
  }

public:
  uint8_t *buffer() const final { return reinterpret_cast<uint8_t *>(handle()->GetMemoryPtr()); }
  size_t total_size() const final { return _shape.num_elements() * sizeof(float); }
  size_t calcOffset(const ir::Coordinates &) const final
  {
    throw std::runtime_error("ICLTensor::calcOffset() is not supported.");
  }
  ir::Layout layout() const final { return ir::Layout::NHWC; }
  ir::DataType data_type() const final { return ir::DataType::FLOAT32; }
  float data_scale() const override
  {
    throw std::runtime_error("ICLTensor::data_scale() is not supported.");
  }
  int32_t data_zero_point() const override
  {
    throw std::runtime_error("ICLTensor::data_zero_point() is not supported.");
  }
  const std::vector<float> &data_scales() const override
  {
    throw std::runtime_error("ICLTensor::data_scales() is not supported.");
  }
  const std::vector<int32_t> &data_zero_points() const override
  {
    throw std::runtime_error("ICLTensor::data_zero_points() is not supported.");
  }
  bool is_dynamic() const override { return false; }
  ir::Shape getShape() const override { return _shape; }
  bool has_padding() const override { return false; }
  void access(const std::function<void(ITensor &tensor)> &fn) final;
  bool needMemoryMap() const final { return true; }
  void enqueueWriteBuffer(const void *ptr, bool blocking = true) final;
  void enqueueReadBuffer(void *ptr, bool blocking = true) final;

  void writeConvertInit();
  void readConvertInit();
  TensorType get_type() { return _type; }

public:
  virtual const tflite::gpu::cl::Tensor *handle() const = 0;
  virtual tflite::gpu::cl::Tensor *handle() = 0;

private:
protected:
  size_t _rank; // Actual rank (reflects extended rank)
  ir::Shape _shape;
  std::shared_ptr<tflite::gpu::cl::Environment> _environment;
  TensorType _type;
  std::unique_ptr<tflite::gpu::TensorObjectConverterBuilder> _converter_builder;
  tflite::gpu::cl::CLMemory _cl_memory;
  std::unique_ptr<tflite::gpu::TensorObjectConverter> _converter_to;
  std::unique_ptr<tflite::gpu::TensorObjectConverter> _converter_from;
};

} // namespace operand
} // namespace gpu_cl
} // namespace backend
} // namespace onert

#endif // __ONERT_BACKEND_GPU_CL_OPERAND_I_CL_TENSOR_H__
