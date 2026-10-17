cmake_minimum_required(VERSION 3.19)

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION VER)
string(STRIP ${VER} VER)

project(
  xdg-desktop-portal-abyss
  DESCRIPTION "An XDG-Destop-Portal backend for Abyssland (and wlroots)"
  VERSION ${VER})

set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
set(SYSTEMD_SERVICES
    ON
    CACHE BOOL "Install systemd service file")

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES DEBUG)
  message(STATUS "Configuring XDPH in Debug with CMake")
  add_compile_definitions(ABYSSLAND_DEBUG)
else()
  add_compile_options(-O3)
  message(STATUS "Configuring XDPH in Release with CMake")
endif()

add_compile_definitions(XDPH_VERSION="${VER}")

include_directories(. "protocols/")

# configure
include(GNUInstallDirs)
set(LIBEXECDIR ${CMAKE_INSTALL_FULL_LIBEXECDIR})
configure_file(org.freedesktop.impl.portal.desktop.abyss.service.in
               org.freedesktop.impl.portal.desktop.abyss.service @ONLY)
if(SYSTEMD_SERVICES)
  configure_file(contrib/systemd/xdg-desktop-portal-abyss.service.in
                 contrib/systemd/xdg-desktop-portal-abyss.service @ONLY)
endif()

set(CMAKE_CXX_STANDARD 23)
add_compile_options(
  -Wall
  -Wextra
  -Wno-unused-parameter
  -Wno-unused-value
  -Wno-missing-field-initializers
  -Wno-narrowing
  -Wno-pointer-arith
  $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-fpermissive>
  -Wno-address-of-temporary)

# dependencies
message(STATUS "Checking deps...")
add_subdirectory(abyssland-share-picker)

find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(
  deps
  REQUIRED
  IMPORTED_TARGET
  wayland-client
  wayland-protocols>=1.31
  libpipewire-0.3>=1.1.82
  libspa-0.2
  libdrm
  uuid
  gbm
  abyssparse>=0.2.0
  abyssutils>=0.2.6
  abyss-scanner>=0.4.2)

# check whether we can find sdbus-c++ through pkg-config
pkg_check_modules(SDBUS IMPORTED_TARGET sdbus-c++>=2.0.0)
if(NOT SDBUS_FOUND)
  include_directories("subprojects/sdbus-cpp/include/")
  add_subdirectory(subprojects/sdbus-cpp EXCLUDE_FROM_ALL)
  add_library(PkgConfig::SDBUS ALIAS sdbus-c++)
endif()

# same for abyssland-protocols
pkg_check_modules(ABYSSLAND_PROTOS IMPORTED_TARGET abyssland-protocols>=0.7.0)
if(ABYSSLAND_PROTOS_FOUND)
  pkg_get_variable(ABYSSLAND_PROTOCOLS abyssland-protocols pkgdatadir)
else()
  set(ABYSSLAND_PROTOCOLS "${CMAKE_SOURCE_DIR}/subprojects/abyssland-protocols")
endif()

file(GLOB_RECURSE SRCFILES CONFIGURE_DEPENDS "src/*.cpp")
add_executable(xdg-desktop-portal-abyss ${SRCFILES})
option(BUILD_TESTING "Build unit tests" OFF)
if(BUILD_TESTING)
  find_package(Qt6 REQUIRED COMPONENTS Widgets)
  enable_testing()
  add_executable(restore_match_test tests/restore_match_test.cpp src/shared/RestoreMatch.cpp)
  add_executable(picker_data_test tests/picker_data_test.cpp src/shared/PickerData.cpp)
  add_executable(restore_data_test tests/restore_data_test.cpp src/shared/RestoreData.cpp)
  add_executable(desktop_identity_test tests/desktop_identity_test.cpp src/helpers/MiscFunctions.cpp)
  target_link_libraries(desktop_identity_test PRIVATE PkgConfig::deps PkgConfig::SDBUS)
  add_executable(picker_gui_test tests/picker_gui_test.cpp src/shared/PickerData.cpp
                 abyssland-share-picker/mainpicker.cpp abyssland-share-picker/mainpicker.h
                 abyssland-share-picker/mainpicker.ui abyssland-share-picker/elidedbutton.cpp)
  set_target_properties(picker_gui_test PROPERTIES AUTOMOC ON AUTOUIC ON)
  target_link_libraries(picker_gui_test PRIVATE Qt6::Widgets PkgConfig::deps)
  target_link_libraries(restore_data_test PRIVATE PkgConfig::SDBUS)
  add_test(NAME restore_match COMMAND restore_match_test)
  add_test(NAME picker_data COMMAND picker_data_test)
  add_test(NAME restore_data COMMAND restore_data_test)
  add_test(NAME desktop_identity COMMAND desktop_identity_test)
  add_test(NAME picker_gui COMMAND picker_gui_test)
endif()

target_link_libraries(
  xdg-desktop-portal-abyss PRIVATE rt PkgConfig::SDBUS Threads::Threads
                                      PkgConfig::deps)

# protocols
pkg_get_variable(WAYLAND_PROTOCOLS_DIR wayland-protocols pkgdatadir)
message(STATUS "Found wayland-protocols at ${WAYLAND_PROTOCOLS_DIR}")
pkg_get_variable(WAYLAND_SCANNER_DIR wayland-scanner pkgdatadir)
message(STATUS "Found wayland-scanner at ${WAYLAND_SCANNER_DIR}")

function(protocolnew protoPath protoName external)
  if(external)
    set(path ${protoPath})
  else()
    set(path ${WAYLAND_PROTOCOLS_DIR}/${protoPath})
  endif()
  add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/protocols/${protoName}.cpp
           ${CMAKE_SOURCE_DIR}/protocols/${protoName}.hpp
    COMMAND abyss-scanner --client ${path}/${protoName}.xml
            ${CMAKE_SOURCE_DIR}/protocols/
    DEPENDS ${path}/${protoName}.xml
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  target_sources(xdg-desktop-portal-abyss PRIVATE protocols/${protoName}.cpp
                                                     protocols/${protoName}.hpp)
endfunction()
function(protocolWayland)
  add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/protocols/wayland.cpp
           ${CMAKE_SOURCE_DIR}/protocols/wayland.hpp
    COMMAND abyss-scanner --wayland-enums --client
            ${WAYLAND_SCANNER_DIR}/wayland.xml ${CMAKE_SOURCE_DIR}/protocols/
    DEPENDS ${WAYLAND_SCANNER_DIR}/wayland.xml
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
  target_sources(xdg-desktop-portal-abyss PRIVATE protocols/wayland.cpp
                                                     protocols/wayland.hpp)
endfunction()

protocolwayland()

protocolnew("${CMAKE_SOURCE_DIR}/protocols"
            "wlr-foreign-toplevel-management-unstable-v1" true)
protocolnew("${CMAKE_SOURCE_DIR}/protocols" "wlr-screencopy-unstable-v1" true)
protocolnew("${ABYSSLAND_PROTOCOLS}/protocols" "abyssland-global-shortcuts-v1"
            true)
protocolnew("${ABYSSLAND_PROTOCOLS}/protocols" "abyssland-toplevel-export-v1"
            true)
protocolnew("${ABYSSLAND_PROTOCOLS}/protocols" "abyssland-toplevel-mapping-v1"
        true)
protocolnew("${ABYSSLAND_PROTOCOLS}/protocols" "abyssland-input-capture-v1"
            true)
protocolnew("stable/linux-dmabuf" "linux-dmabuf-v1" false)
protocolnew("unstable/xdg-output" "xdg-output-unstable-v1" false)
protocolnew("staging/ext-foreign-toplevel-list" "ext-foreign-toplevel-list-v1" false)

# Installation
install(TARGETS abyss-share-picker)
install(TARGETS xdg-desktop-portal-abyss
        DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})

install(FILES abyss.portal
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/xdg-desktop-portal/portals")
install(
  FILES ${CMAKE_BINARY_DIR}/org.freedesktop.impl.portal.desktop.abyss.service
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/dbus-1/services")
if(SYSTEMD_SERVICES)
  install(
    FILES
      ${CMAKE_BINARY_DIR}/contrib/systemd/xdg-desktop-portal-abyss.service
    DESTINATION "lib/systemd/user")
endif()
