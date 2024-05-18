vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO stdware/qwindowkit
    REF 00b91a374872280a4a5addc22f08c7954a6637aa
    SHA512 d9fbbe03be7d8b20d09d2ff984330f5c1813827412a17c606eb03203776aaacdd9b4fdb16eade6391a5c6aa5b2bf373ec4a2421774fbb1eb2f3f32f02462f2ab
)

if (WIN32)
    if (DEFINED ENV{Qt_DIR})
        set(QT_PATH "$ENV{Qt_DIR}" ${CMAKE_PREFIX_PATH})
    elseif (DEFINED ENV{Qt6_DIR})
        set(QT_PATH "$ENV{Qt6_DIR}" ${CMAKE_PREFIX_PATH})
    elseif (DEFINED ENV{Qt5_DIR})
        set(QT_PATH "$ENV{Qt5_DIR}" ${CMAKE_PREFIX_PATH})
    elseif (DEFINED ENV{PATH})
        foreach (PATH_ITEM $ENV{PATH})
            string(TOLOWER "${PATH_ITEM}" PATH_ITEM_LOWER)
            if (PATH_ITEM_LOWER MATCHES "qt")
                set(CMAKE_PREFIX_PATH "${PATH_ITEM}" ${CMAKE_PREFIX_PATH})
                break()
            endif ()
        endforeach ()
    endif ()
endif ()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
    -DQWINDOWKIT_BUILD_STATIC=OFF
    -DQWINDOWKIT_BUILD_WIDGETS=ON
    -DQWINDOWKIT_BUILD_QUICK=OFF
    -DCMAKE_PREFIX_PATH=${QT_PATH}
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(
    PACKAGE_NAME ${PORT}
    CONFIG_PATH lib/cmake/QWindowKit
)
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}/" RENAME copyright)