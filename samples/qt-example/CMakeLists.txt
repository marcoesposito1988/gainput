
project(qtsample)

find_package(Qt5 QUIET COMPONENTS Widgets)

if (Qt5_FOUND)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    if(CMAKE_VERSION VERSION_LESS "3.7.0")
        set(CMAKE_INCLUDE_CURRENT_DIR ON)
    endif()

    set(sources main.cpp widget.cpp widget.h widget.ui)

    if (WIN32)
        add_executable(qtsample WIN32 ${sources})
    else()
        add_executable(qtsample ${sources})
    endif()

    target_include_directories(qtsample PRIVATE ../samplefw/)
    target_link_libraries(qtsample PRIVATE gainput Qt5::Widgets)

    if(BUILD_SHARED_LIBS)
        # Copy the output of gainput shared library to output folder.
        add_custom_command( TARGET qtsample
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:gainput> $<TARGET_FILE_DIR:qtsample>
                )
    endif(BUILD_SHARED_LIBS)

endif(Qt5_FOUND)