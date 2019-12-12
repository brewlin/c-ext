dnl config.m4 for extension shamem

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(shamem, for shamem support,
dnl Make sure that the comment is aligned:
dnl [  --with-shamem             Include shamem support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(shamem, whether to enable shamem support,
dnl Make sure that the comment is aligned:
[  --enable-shamem          Enable shamem support], no)

if test "$PHP_SHAMEM" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=\`$PKG_CONFIG foo --cflags\`
  dnl     LIBFOO_LIBDIR=\`$PKG_CONFIG foo --libs\`
  dnl     LIBFOO_VERSON=\`$PKG_CONFIG foo --modversion\`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, SHAMEM_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-shamem -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/shamem.h"  # you most likely want to change this
  dnl if test -r $PHP_SHAMEM/$SEARCH_FOR; then # path given as parameter
  dnl   SHAMEM_DIR=$PHP_SHAMEM
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for shamem files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       SHAMEM_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$SHAMEM_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the shamem distribution])
  dnl fi

  dnl # --with-shamem -> add include path
  dnl PHP_ADD_INCLUDE($SHAMEM_DIR/include)

  dnl # --with-shamem -> check for lib and symbol presence
  dnl LIBNAME=SHAMEM # you may want to change this
  dnl LIBSYMBOL=SHAMEM # you most likely want to change this

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SHAMEM_DIR/$PHP_LIBDIR, SHAMEM_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_SHAMEMLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong shamem lib version or lib not found])
  dnl ],[
  dnl   -L$SHAMEM_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(SHAMEM_SHARED_LIBADD)

  dnl # In case of no dependencies
  AC_DEFINE(HAVE_SHAMEM, 1, [ Have shamem support ])

  source_file="\
      shamem.c \
      lib_shamem.c
  "

  PHP_NEW_EXTENSION(shamem, $source_file, $ext_shared)
  PHP_INSTALL_HEADERS([ext/lib], [*.h config.h include/*.h])
fi
