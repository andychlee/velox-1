/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "velox/core/FunctionRegistry.h"
#include "velox/expression/VectorFunctionBindings.h"

namespace facebook::velox {

template <typename Func, typename TReturn, typename... TArgs>
void registerFunction(
    const std::vector<std::string>& aliases = {},
    std::shared_ptr<const Type> returnType = nullptr) {
  using funcClass = typename Func::template udf<core::DynamicExec>;
  // register basic
  using holderClass =
      core::UDFHolder<funcClass, core::DynamicExec, TReturn, TArgs...>;
  core::registerFunction<holderClass>(aliases, returnType);

  // register Vector
  using VectorHolderClass = core::UDFHolder<
      typename Func::template udf<exec::VectorExec>,
      exec::VectorExec,
      TReturn,
      TArgs...>;
  exec::registerVectorFunction<VectorHolderClass>(aliases, move(returnType));
}

// New registration function; mostly a copy from the function above, but taking
// the inner "udf" struct directly, instead of the wrapper. We can keep both for
// a while to maintain backwards compatibility, but the idea is to remove the
// one above eventually.
template <template <class> typename Func, typename TReturn, typename... TArgs>
void registerFunction(
    const std::vector<std::string>& aliases = {},
    std::shared_ptr<const Type> returnType = nullptr) {
  using funcClass = Func<core::DynamicExec>;
  using holderClass =
      core::UDFHolder<funcClass, core::DynamicExec, TReturn, TArgs...>;
  core::registerFunction<holderClass>(aliases, returnType);
  using VectorHolderClass = core::
      UDFHolder<Func<exec::VectorExec>, exec::VectorExec, TReturn, TArgs...>;
  exec::registerVectorFunction<VectorHolderClass>(aliases, move(returnType));
}

} // namespace facebook::velox
