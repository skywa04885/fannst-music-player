CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(MPEGReader)

ADD_LIBRARY(
	${PROJECT_NAME}_Play STATIC

	classes/MSWav.class.cpp
	classes/MSWav.class.hpp

	classes/AudioHandler.class.hpp
	classes/AudioHandler.class.cpp

	classes/ProgressBar.class.hpp
	classes/ProgressBar.class.cpp
)

ADD_LIBRARY(
	${PROJECT_NAME}_Logger STATIC

	classes/Logger.class.hpp
	classes/Logger.class.cpp
)

ADD_EXECUTABLE(
  ${PROJECT_NAME}

  main.cpp
  main.hpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_NAME}_Logger ${PROJECT_NAME}_Play -lasound)
