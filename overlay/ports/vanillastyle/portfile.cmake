vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO AlanusMeminius/VanillaStyle
       REF 26ed5305d018e1ea554bb4ac3bd367524b893a41
       SHA512 9a5d9d632a9e89150576a2782d5b693a437fe1ce4bc9b4302bf265475b18c11b0018bb0152c5a99667103b6b2d4f010f728e85a851f8f1a02c38584a7c841e88
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