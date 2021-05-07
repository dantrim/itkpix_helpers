file(GLOB execfiles [a-zA-Z]*.cpp)

#list(APPEND libs Util Yarr Spec Rd53b)
#set(VARIANTINCLUDE ${CMAKE_BINARY_DIR}/YARR/src/external/src/variant/include)
#
#list(APPEND YARRINCLUDES "")
#foreach(lib ${libs})
#    file(GLOB files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/YARR/src/lib${lib}/include)
#    list(APPEND YARRINCLUDES ${files})
#endforeach()


foreach(target ${execfiles})
    get_filename_component(execname ${target} NAME_WE)
    get_filename_component(srcfile ${target} NAME)
    message("FOO executable ${execname}")
    add_executable(${execname} ${srcfile})
    target_link_libraries(${execname} itkpix_helpers)
    #target_include_directories(${execname} PRIVATE ${YARRINCLUDES} ${VARIANTINCLUDE})
endforeach()
