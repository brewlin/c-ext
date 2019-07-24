PHP_ARG_ENABLE(coroutine, whether to enable coroutine support,
Make sure that the comment is aligned:
[  --enable-coroutine           Enable coroutine support])

# AC_CANONICAL_HOST

if test "$PHP_COROUTINE" != "no"; then
    PHP_ADD_LIBRARY(pthread)
    COROUTINE_ASM_DIR="thirdparty/boost/asm/"
    CFLAGS="-Wall -pthread $CFLAGS"

    AS_CASE([$host_os],
        [linux*], [COROUTINE_OS="LINUX"],
        []
    )

    AS_CASE([$host_cpu],
      [x86_64*], [COROUTINE_CPU="x86_64"],
      [x86*], [COROUTINE_CPU="x86"],
      [i?86*], [COROUTINE_CPU="x86"],
      [arm*], [COROUTINE_CPU="arm"],
      [aarch64*], [COROUTINE_CPU="arm64"],
      [arm64*], [COROUTINE_CPU="arm64"],
      []
    )

    if test "$COROUTINE_CPU" = "x86_64"; then
        if test "$COROUTINE_OS" = "LINUX"; then
            COROUTINE_CONTEXT_ASM_FILE="x86_64_sysv_elf_gas.S"
        fi
    elif test "$COROUTINE_CPU" = "x86"; then
        if test "$COROUTINE_OS" = "LINUX"; then
            COROUTINE_CONTEXT_ASM_FILE="i386_sysv_elf_gas.S"
        fi
    elif test "$COROUTINE_CPU" = "arm"; then
        if test "$COROUTINE_OS" = "LINUX"; then
            COROUTINE_CONTEXT_ASM_FILE="arm_aapcs_elf_gas.S"
        fi
    elif test "$COROUTINE_CPU" = "arm64"; then
        if test "$COROUTINE_OS" = "LINUX"; then
            COROUTINE_CONTEXT_ASM_FILE="arm64_aapcs_elf_gas.S"
        fi
    elif test "$COROUTINE_CPU" = "mips32"; then
        if test "$COROUTINE_OS" = "LINUX"; then
           COROUTINE_CONTEXT_ASM_FILE="mips32_o32_elf_gas.S"
        fi
    fi

    coroutine_source_file="\
        coroutine.cc \
        php_coroutine.cc \
        php_coroutine_util.cc \
        ${COROUTINE_ASM_DIR}make_${COROUTINE_CONTEXT_ASM_FILE} \
        ${COROUTINE_ASM_DIR}jump_${COROUTINE_CONTEXT_ASM_FILE}
    "

    PHP_NEW_EXTENSION(coroutine, $coroutine_source_file, $ext_shared, ,, cxx)

    PHP_ADD_INCLUDE([$ext_srcdir])
    PHP_ADD_INCLUDE([$ext_srcdir/include])

    PHP_INSTALL_HEADERS([ext/coroutine], [*.h config.h include/*.h thirdparty/*.h])

    PHP_REQUIRE_CXX()

    CXXFLAGS="$CXXFLAGS -Wall -Wno-unused-function -Wno-deprecated -Wno-deprecated-declarations"
    CXXFLAGS="$CXXFLAGS -std=c++11"

    PHP_ADD_BUILD_DIR($ext_builddir/thirdparty/boost)
    PHP_ADD_BUILD_DIR($ext_builddir/thirdparty/boost/asm)
fi