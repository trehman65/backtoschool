cmake_minimum_required(VERSION 3.5)
project(backtoschool)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES image-histogram.h
        bg-estimate.h
        bg-estimate.cc
        BasicLevenshteinDistance.cc
        BasicLevenshteinDistance.h
        image-histogram.h
        TextDistanceInterface.h)

add_executable(Segmentation main-segmentation.cpp ${SOURCE_FILES})
add_executable(Evaluation main-evaluation.cpp ${SOURCE_FILES})

target_link_libraries(Segmentation lept tesseract opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs pthread ${Boost_LIBRARIES})
target_link_libraries(Evaluation lept tesseract opencv_core opencv_imgproc opencv_highgui opencv_imgcodecs pthread ${Boost_LIBRARIES})