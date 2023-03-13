# Generated by Boost 1.81.0

# address-model=64

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "64 bit, need 32")
  return()
endif()

# layout=versioned

# toolset=mgw12

if(Boost_COMPILER)
  if(NOT "mgw12" IN_LIST Boost_COMPILER AND NOT "-mgw12" IN_LIST Boost_COMPILER)
    _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "mgw12, Boost_COMPILER=${Boost_COMPILER}")
    return()
  endif()
else()
  if(BOOST_DETECTED_TOOLSET AND NOT BOOST_DETECTED_TOOLSET STREQUAL "mgw12")
    _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "mgw12, detected ${BOOST_DETECTED_TOOLSET}, set Boost_COMPILER to override")
    return()
  endif()
endif()

# link=shared

if(DEFINED Boost_USE_STATIC_LIBS)
  if(Boost_USE_STATIC_LIBS)
    _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "shared, Boost_USE_STATIC_LIBS=${Boost_USE_STATIC_LIBS}")
    return()
  endif()
else()
  if(WIN32 AND NOT _BOOST_SINGLE_VARIANT)
    _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "shared, default on Windows is static, set Boost_USE_STATIC_LIBS=OFF to override")
    return()
  endif()
endif()

# runtime-link=shared

if(Boost_USE_STATIC_RUNTIME)
  _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "shared runtime, Boost_USE_STATIC_RUNTIME=${Boost_USE_STATIC_RUNTIME}")
  return()
endif()

# runtime-debugging=off

if(Boost_USE_DEBUG_RUNTIME)
  _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "release runtime, Boost_USE_DEBUG_RUNTIME=${Boost_USE_DEBUG_RUNTIME}")
  return()
endif()

# threading=multi

if(DEFINED Boost_USE_MULTITHREADED AND NOT Boost_USE_MULTITHREADED)
  _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "multithreaded, Boost_USE_MULTITHREADED=${Boost_USE_MULTITHREADED}")
  return()
endif()

# variant=release

if(NOT "${Boost_USE_RELEASE_LIBS}" STREQUAL "" AND NOT Boost_USE_RELEASE_LIBS)
  _BOOST_SKIPPED("libboost_serialization-mgw12-mt-x64-1_81.dll.a" "release, Boost_USE_RELEASE_LIBS=${Boost_USE_RELEASE_LIBS}")
  return()
endif()

if(Boost_VERBOSE OR Boost_DEBUG)
  message(STATUS "  [x] libboost_serialization-mgw12-mt-x64-1_81.dll.a")
endif()

# Create imported target Boost::serialization

if(NOT TARGET Boost::serialization)
  add_library(Boost::serialization SHARED IMPORTED)

  set_target_properties(Boost::serialization PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${_BOOST_INCLUDEDIR}"
    INTERFACE_COMPILE_DEFINITIONS "BOOST_SERIALIZATION_NO_LIB"
  )
endif()

# Target file name: libboost_serialization-mgw12-mt-x64-1_81.dll.a

get_target_property(__boost_imploc Boost::serialization IMPORTED_IMPLIB_RELEASE)
if(__boost_imploc)
  message(SEND_ERROR "Target Boost::serialization already has an imported location '${__boost_imploc}', which is being overwritten with '${_BOOST_LIBDIR}/libboost_serialization-mgw12-mt-x64-1_81.dll.a'")
endif()
unset(__boost_imploc)

set_property(TARGET Boost::serialization APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)

set_target_properties(Boost::serialization PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE CXX
  IMPORTED_IMPLIB_RELEASE "${_BOOST_LIBDIR}/libboost_serialization-mgw12-mt-x64-1_81.dll.a"
  )

set_target_properties(Boost::serialization PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_BOOST_LIBDIR}/libboost_serialization-mgw12-mt-x64-1_81.dll"
  )

set_target_properties(Boost::serialization PROPERTIES
  MAP_IMPORTED_CONFIG_MINSIZEREL Release
  MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
  )

set_property(TARGET Boost::serialization APPEND
  PROPERTY INTERFACE_COMPILE_DEFINITIONS "BOOST_SERIALIZATION_DYN_LINK"
  )

list(APPEND _BOOST_SERIALIZATION_DEPS headers)
