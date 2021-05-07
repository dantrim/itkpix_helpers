
list(APPEND LIBS itkpix_helpers)
foreach(lib ${LIBS})
    include_directories(${lib})
    file(GLOB SRCFILES
        CONFIGURE_DEPENDS
        ${lib}*.cpp
    )
    list(APPEND LIBSOURCES ${SRCFILES})
endforeach()

add_library(itkpix_helpers SHARED)
target_sources(itkpix_helpers PRIVATE ${LIBSOURCES})
target_include_directories(itkpix_helpers PUBLIC ${PROJECT_SOURCE_DIR}/itkpix_helpers/src)

list(APPEND YARRLIBS Util Yarr Spec Rd53b)
set(VARIANTINCLUDE ${CMAKE_BINARY_DIR}/YARR/src/external/src/variant/include)
list(APPEND YARRSRCFILES "")
foreach(lib ${YARRLIBS})
    set(libpath ${PROJECT_SOURCE_DIR}/YARR/src/lib${lib}/include)
    target_include_directories(itkpix_helpers PUBLIC ${libpath})
    file(GLOB srcfiles
        ${PROJECT_SOURCE_DIR}/YARR/src/lib${lib}/*.cpp)
    list(APPEND YARRSRCFILES ${srcfiles})
endforeach()
target_include_directories(itkpix_helpers PUBLIC ${VARIANTINCLUDE})

set(YARR_LINK_COMMAND Yarr Util Spec Rd53b pthread)
target_link_libraries(itkpix_helpers ${YARR_LINK_COMMAND})



##
## executables
##
add_subdirectory(tools)
