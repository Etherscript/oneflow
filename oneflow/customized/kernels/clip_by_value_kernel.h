#ifndef ONEFLOW_CUSTOMIZED_KERNELS_CLIP_BY_VALUE_KERNEL_H_
#define ONEFLOW_CUSTOMIZED_KERNELS_CLIP_BY_VALUE_KERNEL_H_

#include "oneflow/core/common/data_type.h"
#include "oneflow/core/device/device_context.h"

namespace oneflow {

template<typename T>
OF_DEVICE_FUNC T DeviceMin(T a, T b) {
#if defined(__CUDA_ARCH__)
  return min(a, b);
#else
  return std::min(a, b);
#endif
}

template<typename T>
OF_DEVICE_FUNC T DeviceMax(T a, T b) {
#if defined(__CUDA_ARCH__)
  return max(a, b);
#else
  return std::max(a, b);
#endif
}

template<typename T>
struct ClipByMinFunctor {
  ClipByMinFunctor(T min) : min_value(min) {}
  OF_DEVICE_FUNC T operator()(T value) { return DeviceMax(value, min_value); }
  T min_value;
};

template<typename T>
struct ClipByMaxFunctor {
  ClipByMaxFunctor(T max) : max_value(max) {}
  OF_DEVICE_FUNC T operator()(T value) { return DeviceMin(value, max_value); }
  T max_value;
};

template<typename T>
struct ClipByMinMaxFunctor {
  ClipByMinMaxFunctor(T min, T max) : min_value(min), max_value(max) {}
  OF_DEVICE_FUNC T operator()(T value) { return DeviceMin(DeviceMax(value, min_value), max_value); }
  T min_value;
  T max_value;
};

template<typename T>
struct ClipByMinGradFunctor {
  ClipByMinGradFunctor(T min) : min_value(min) {}
  OF_DEVICE_FUNC T operator()(T value, T grad) {
    return value < min_value ? static_cast<T>(0) : grad;
  }
  T min_value;
};

template<typename T>
struct ClipByMaxGradFunctor {
  ClipByMaxGradFunctor(T max) : max_value(max) {}
  OF_DEVICE_FUNC T operator()(T value, T grad) {
    return value > max_value ? static_cast<T>(0) : grad;
  }
  T max_value;
};

template<typename T>
struct ClipByMinMaxGradFunctor {
  ClipByMinMaxGradFunctor(T min, T max) : min_value(min), max_value(max) {}
  OF_DEVICE_FUNC T operator()(T value, T grad) {
    return (value < min_value || value > max_value) ? static_cast<T>(0) : grad;
  }
  T min_value;
  T max_value;
};

template<DeviceType device_type, typename T>
struct ClipKernelUtil {
  template<typename F>
  static void Forward(DeviceCtx* ctx, F clip_func, const int64_t n, const T* x, T* y);
  template<typename F>
  static void Backward(DeviceCtx* ctx, F clip_func, const int64_t n, const T* x, const T* dy,
                       T* dx);
};

}  // namespace oneflow

#endif  // ONEFLOW_CUSTOMIZED_KERNELS_CLIP_BY_VALUE_KERNEL_H_
