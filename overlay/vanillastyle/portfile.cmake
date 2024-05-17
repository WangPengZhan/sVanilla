vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

find_program(GIT git)

set(GIT_URL "https://github.com/AlanusMeminius/VanillaStyle.git")
set(GIT_REV "8d246c7069d51e6ce5c5e5075c600c486a6a7453")

set(SOURCE_PATH ${CURRENT_BUILDTREES_DIR}/src/${PORT})

if (NOT EXISTS ${SOURCE_PATH})
    message(STATUS "Directory '${SOURCE_PATH}' does not exist: creating it")
    file(MAKE_DIRECTORY ${SOURCE_PATH})
endif ()


if(NOT EXISTS "${SOURCE_PATH}/.git")
    message(STATUS "Cloning and fetching submodules")
    vcpkg_execute_required_process(
            COMMAND ${GIT} clone --recurse-submodules ${GIT_URL} ${SOURCE_PATH}
            WORKING_DIRECTORY ${SOURCE_PATH}
            LOGNAME clone
    )

    message(STATUS "Checkout revision ${GIT_REV}")
    vcpkg_execute_required_process(
            COMMAND ${GIT} checkout ${GIT_REV}
            WORKING_DIRECTORY ${SOURCE_PATH}
            LOGNAME checkout
    )
endif()

vcpkg_cmake_configure(
        SOURCE_PATH "${SOURCE_PATH}"
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
        PACKAGE_NAME "VanillaStyle"
        CONFIG_PATH "lib/cmake/VanillaStyle"
)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
#configure_file("${CMAKE_CURRENT_LIST_DIR}/usage" "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage" COPYONLY)