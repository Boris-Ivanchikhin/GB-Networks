cmake_minimum_required(VERSION 3.10)

project(boost-tcp-echo-server C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

file(GLOB ${PROJECT_NAME}_SRC
        *.h *.cpp
)

source_group(source FILES ${${PROJECT_NAME}_SRC}})

if (WIN32)
    # Locally-installed.
    set(Boost_NO_SYSTEM_PATHS ON)

    set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/boost_1_81_0")
    set(Boost_INCLUDE_DIR ${BOOST_ROOT}/include)
    set(Boost_INCLUDE_DIRS ${BOOST_ROOT}/include)
    set(Boost_LIBRARY_DIRS ${BOOST_ROOT}/lib)

    link_directories("${Boost_LIBRARY_DIRS}")

endif()

if(NOT WIN32)
    find_package(Boost
            REQUIRED COMPONENTS
            system program_options thread
            stacktrace_basic stacktrace_backtrace stacktrace_addr2line stacktrace_noop
            OPTIONAL_COMPONENTS
            regex
    )
else()
    find_package(Boost
            REQUIRED COMPONENTS
            system program_options thread
            OPTIONAL_COMPONENTS
            regex
            )
endif() # find_package

message(status "** Boost FOUND [${PROJECT_NAME}]: ${Boost_FOUND}")

if(Boost_FOUND)
  include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
  add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")

  set_target_properties(${PROJECT_NAME}
          PROPERTIES
          CXX_STANDARD 17
          CXX_STANDARD_REQUIRED ON

          Boost_USE_STATIC_LIBS    ON  # find static libs
          Boost_USE_MULTITHREADED  ON
          Boost_USE_STATIC_RUNTIME OFF
  )

  if (NOT WIN32)
      target_link_libraries(${PROJECT_NAME}
              # Boost
              PUBLIC ${Boost_LIBRARIES}
              # Boost::stacktrace
              ${Boost_STACKTRACE_BASIC_LIBRARY}
              ${Boost_STACKTRACE_BACKTRACE_LIBRARY}
              ${Boost_STACKTRACE_ADDR2LINE_LIBRARY}
              ${Boost_STACKTRACE_NOOP_LIBRARY}
              pthread dl
          )
  else()
      target_link_libraries(${PROJECT_NAME}
              # Boost
              PUBLIC ${Boost_LIBRARIES}
              wsock32 ws2_32
              )
  endif()

  # Define Boost_USE_DEBUG_LIBS and Boost_USE_RELEASE_LIBS 
  # Check if a multi-config generator is used.
  if(GENERATOR_IS_MULTI_CONFIG)
	# A multi-configuration generator is used. Use debug and release libs.
    set_target_properties(
      ${PROJECT_NAME} PROPERTIES
	    Boost_USE_DEBUG_LIBS   ON
	    Boost_USE_RELEASE_LIBS ON
    )

  else()
	# If not, check which kind of configuration is beeing built.
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
      set_target_properties(
        ${PROJECT_NAME} PROPERTIES
	    # Only debug libraries are needed.
	    Boost_USE_DEBUG_LIBS   ON
	    Boost_USE_RELEASE_LIBS OFF
        )
	elseif(${CMAKE_BUILD_TYPE} STREQUAL  "RelWithDebInfo")
	  # Both debug and release libraries are needed.
      set_target_properties(
        ${PROJECT_NAME} PROPERTIES
	    Boost_USE_DEBUG_LIBS   ON
	    Boost_USE_RELEASE_LIBS ON
        )
	else()
      set_target_properties(
        ${PROJECT_NAME} PROPERTIES
	    # Only release libraries are needed.
	    Boost_USE_DEBUG_LIBS   OFF
        Boost_USE_RELEASE_LIBS ON
        )
	endif()

  endif() # Define Boost_USE_DEBUG_LIBS

#  if(WIN32)
#      target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
#  endif()

else() # if(Boost_FOUND)

    message(status "** Boost Root: ${BOOST_ROOT}")
    message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
    message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
    message(status "** Boost Libraries: ${Boost_LIBRARIES}")

endif() # if(Boost_FOUND)



