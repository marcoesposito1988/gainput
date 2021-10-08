
project(basicsample)

if(GAINPUT_PLATFORM_ANDROID)
        add_library(basicsample SHARED basicsample_android.cpp basicsample_android_generic.cpp)
elseif(GAINPUT_PLATFORM_IOS)
  add_executable(basicsample "MACOSX_BUNDLE"
      basicsample_ios.mm
      "${CMAKE_CURRENT_LIST_DIR}/../../extern/ios/Launch Screen.storyboard"
  )
  set_target_properties( basicsample PROPERTIES
    XCODE_ATTRIBUTE_INFOPLIST_FILE "${CMAKE_CURRENT_LIST_DIR}/../../extern/ios/Info.plist"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
  )
  target_link_libraries(basicsample ${UIKIT} ${QUARTZCORE})
  target_link_libraries(basicsample ${FOUNDATION} ${IOKIT} ${APPKIT} ${GAME_CONTROLLER})
elseif(GAINPUT_PLATFORM_MAC)
  add_executable(basicsample basicsample_mac.mm)
  target_link_libraries(basicsample ${FOUNDATION} ${IOKIT} ${APPKIT} ${GAME_CONTROLLER})
elseif(GAINPUT_PLATFORM_LINUX)
    if(GAINPUT_ENABLE_XI2)
        add_executable(basicsample basicsample_linux_xi2.cpp)
        target_link_libraries(basicsample X11 GL rt Xext Xi)
    else()
        add_executable(basicsample basicsample_linux.cpp)
        target_link_libraries(basicsample X11 GL rt)
    endif()
elseif(GAINPUT_PLATFORM_WIN)
        add_executable(basicsample WIN32 basicsample_win.cpp)
        target_link_libraries(basicsample ${XINPUT} ws2_32)
endif()

target_link_libraries(basicsample gainput)

if(BUILD_SHARED_LIBS)
  # Copy the output of gainput shared library to output folder.
  add_custom_command( TARGET basicsample 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:gainput> $<TARGET_FILE_DIR:basicsample>
  )
endif(BUILD_SHARED_LIBS)
