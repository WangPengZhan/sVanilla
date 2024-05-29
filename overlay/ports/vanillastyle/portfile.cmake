vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO AlanusMeminius/VanillaStyle
    REF 856ee4009a10e5e8a99bdebc0ad48347c1451c70
    SHA512 b42cffe2be413a02a32ef59c4712471352413c986ad19a55788bf3410da5e7a5e17699c8fee733ad089904549ae0e946b94208c0e98571dbb08eed0f3f364a02
    PATCHES
    dependence.patch
)

vcpkg_download_distfile(
    HEADER_FILE
    URLS "https://raw.githubusercontent.com/bfraboni/FastGaussianBlur/main/fast_gaussian_blur_template.h"
    FILENAME "fast_gaussian_blur_template.h"
    SHA512 6c95bfdecdc5c55c3e4c467ea97c90c4a0a8c903257e8e5c76853195282c74b3f4029745e3631d319ac6fe314d9f2a471b01b6f640f634799ba98f927b4ab1f0
)
file(COPY
    ${HEADER_FILE}
    DESTINATION "${SOURCE_PATH}/${PORT}/include"
)

if (WIN32)
    if (DEFINED ENV{QT_ROOT_DIR})
        set(QT_PATH "$ENV{Qt_DIR}" ${CMAKE_PREFIX_PATH})
    elseif (DEFINED ENV{Qt_DIR})
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
    -DCMAKE_PREFIX_PATH=${QT_PATH}
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    PACKAGE_NAME ${PORT}
    CONFIG_PATH "cmake"
)
vcpkg_copy_pdbs()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
#configure_file("${CMAKE_CURRENT_LIST_DIR}/usage" "${CURRENT_PACKAGES_DIR}/share/${PORT}/usage" COPYONLY)