# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/bartoszkawa/Desktop/astudia/Sensory/esp/esp-idf/components/bootloader/subproject"
  "/Users/bartoszkawa/Desktop/astudia/Sensory/plant-watering/build/bootloader"
  "/Users/bartoszkawa/Desktop/astudia/Sensory/plant-watering/build/bootloader-prefix"
  "/Users/bartoszkawa/Desktop/astudia/Sensory/plant-watering/build/bootloader-prefix/tmp"
  "/Users/bartoszkawa/Desktop/astudia/Sensory/plant-watering/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/bartoszkawa/Desktop/astudia/Sensory/plant-watering/build/bootloader-prefix/src"
  "/Users/bartoszkawa/Desktop/astudia/Sensory/plant-watering/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/bartoszkawa/Desktop/astudia/Sensory/plant-watering/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/bartoszkawa/Desktop/astudia/Sensory/plant-watering/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
