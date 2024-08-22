// Copyright (C) 2024 neverlosecc
// See end of file for extended copyright information.
#include <cassert>
#include <sdk/interfaces/common/CBufferString.h>
#include <tools/loader/loader.h>

void CBufferString::MoveFrom(CBufferString& src) {
    using func_t = void (*)(CBufferString*, CBufferString&);
    static const auto func = []() -> func_t {
        const auto module_handle = loader::find_module_handle("tier0");
        const auto exported_fn = loader::find_module_symbol(module_handle, "?MoveFrom@CBufferString@@QEAAXAEAV1@@Z");
        assert(exported_fn.has_value());
        return reinterpret_cast<func_t>(*exported_fn);
    }();

    return func(this, src);
}

// source2gen - Source2 games SDK generator
// Copyright 2024 neverlosecc
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
