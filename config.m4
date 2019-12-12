PHP_ARG_ENABLE(LIB, whether to enable LIB support,
Make sure that the comment is aligned:
[  --enable-LIB           Enable LIB support])

# AC_CANONICAL_HOST

if test "$PHP_LIB" != "no"; then
    PHP_ADD_LIBRARY(pthread)
    LIB_ASM_DIR="thirdparty/boost/asm/"
    CFLAGS="-Wall -pthread $CFLAGS"

    AS_CASE([$host_os],
        [linux*], [LIB_OS="LINUX"],
        []
    )

    AS_CASE([$host_cpu],
      [x86_64*], [LIB_CPU="x86_64"],
      [x86*], [LIB_CPU="x86"],
      [i?86*], [LIB_CPU="x86"],
      [arm*], [LIB_CPU="arm"],
      [aarch64*], [LIB_CPU="arm64"],
      [arm64*], [LIB_CPU="arm64"],
      []
    )

    if test "$LIB_CPU" = "x86_64"; then
        if test "$LIB_OS" = "LINUX"; then
            LIB_CONTEXT_ASM_FILE="x86_64_sysv_elf_gas.S"
        fi
    elif test "$LIB_CPU" = "x86"; then
        if test "$LIB_OS" = "LINUX"; then
            LIB_CONTEXT_ASM_FILE="i386_sysv_elf_gas.S"
        fi
    elif test "$LIB_CPU" = "arm"; then
        if test "$LIB_OS" = "LINUX"; then
            LIB_CONTEXT_ASM_FILE="arm_aapcs_elf_gas.S"
        fi
    elif test "$LIB_CPU" = "arm64"; then
        if test "$LIB_OS" = "LINUX"; then
            LIB_CONTEXT_ASM_FILE="arm64_aapcs_elf_gas.S"
        fi
    elif test "$LIB_CPU" = "mips32"; then
        if test "$LIB_OS" = "LINUX"; then
           LIB_CONTEXT_ASM_FILE="mips32_o32_elf_gas.S"
        fi
    fi

    lib_source_file="\
        lib.cc \
        ${LIB_ASM_DIR}make_${LIB_CONTEXT_ASM_FILE} \
        ${LIB_ASM_DIR}jump_${LIB_CONTEXT_ASM_FILE}
    "

    PHP_NEW_EXTENSION(lib, $lib_source_file, $ext_shared, ,, cxx)

    PHP_ADD_INCLUDE([$ext_srcdir])
    PHP_ADD_INCLUDE([$ext_srcdir/include])

    PHP_INSTALL_HEADERS([ext/lib], [*.h config.h include/*.h thirdparty/*.h])

    PHP_REQUIRE_CXX()

    CXXFLAGS="$CXXFLAGS -Wall -Wno-unused-function -Wno-deprecated -Wno-deprecated-declarations"
    CXXFLAGS="$CXXFLAGS -std=c++11"

    PHP_ADD_BUILD_DIR($ext_builddir/thirdparty/boost)
    PHP_ADD_BUILD_DIR($ext_builddir/thirdparty/boost/asm)
fi