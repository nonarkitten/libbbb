# Copyright 2019 Jacques Supcik, HEIA-FR
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# CMake toolchain file for lecture "embedded systems 1 & 2"
#

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

UNSET(CMAKE_C_FLAGS CACHE)
UNSET(CMAKE_ASM_FLAGS CACHE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mlittle-endian -funsigned-char -mno-unaligned-access")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-a8 -mtune=cortex-a8 -msoft-float" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -mcpu=cortex-a8" CACHE STRING "" FORCE)
