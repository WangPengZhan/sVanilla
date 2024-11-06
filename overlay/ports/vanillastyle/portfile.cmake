vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO AlanusMeminius/VanillaStyle
        REF 4db800cf4c8a7a048832287979d1a53575952b79
        SHA512 97dc23952590649380cdd3bad375a998d4d71fc190cd8660320750f60b5e5a5e8782580706879c0cb4b7b325700b72c711766512566423a9464207cbb2a26478
    PATCHES
    dependence.patch
)

vcpkg_download_distfile(
    HEADER_FILE
    URLS "https://github.com/bfraboni/FastGaussianBlur/raw/49f0128469c3df89d728d3b94457108682682825/fast_gaussian_blur_template.h"
    FILENAME "fast_gaussian_blur_template.h"
        SHA512 9e92c67de2e8dedbdd080176cf60148b3bc8bf749ef4410438cf1901a27991bd98522eb9b472b6ae73f62d2781e09cca9a880a6563f9c68526d57326b88e31c5
)
file(COPY
    ${HEADER_FILE}
    DESTINATION "${SOURCE_PATH}/${PORT}/include"
)

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