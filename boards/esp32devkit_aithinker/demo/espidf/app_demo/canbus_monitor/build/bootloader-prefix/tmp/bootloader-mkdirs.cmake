# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/Administrator/esp/esp-idf-v4.4.4/components/bootloader/subproject"
  "F:/hello_esp32/2_examples/esp32devkit/espidf/app_demo/canbus_monitor/build/bootloader"
  "F:/hello_esp32/2_examples/esp32devkit/espidf/app_demo/canbus_monitor/build/bootloader-prefix"
  "F:/hello_esp32/2_examples/esp32devkit/espidf/app_demo/canbus_monitor/build/bootloader-prefix/tmp"
  "F:/hello_esp32/2_examples/esp32devkit/espidf/app_demo/canbus_monitor/build/bootloader-prefix/src/bootloader-stamp"
  "F:/hello_esp32/2_examples/esp32devkit/espidf/app_demo/canbus_monitor/build/bootloader-prefix/src"
  "F:/hello_esp32/2_examples/esp32devkit/espidf/app_demo/canbus_monitor/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "F:/hello_esp32/2_examples/esp32devkit/espidf/app_demo/canbus_monitor/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
