SUBDIRS = src tests samples

EXTRA_DIST = version.incl \
             xerces-c.spec \
             xerces-c.pc.in \
             CREDITS \
             INSTALL \
             KEYS \
             LICENSE \
             NOTICE \
             README \
             config/pretty-make \
             doc \
             projects \
             reconf \
             scripts/createBindingsFormatFromHPP.pl \
             scripts/makeStringDefinition.pl \
             scripts/sanityTest.pl \
             scripts/sanityTest_ExpectedResult.log \
             tools

AUTOMAKE_OPTIONS = foreign dist-bzip2 dist-zip dist-xz
ACLOCAL_AMFLAGS = -I m4

CLEANFILES = test-results.log

pkgconfigdir = @pkgconfigdir@
pkgconfig_DATA= xerces-c.pc

check:
	-rm -f test-results.log
	export PATH=${abs_top_builddir}/samples:${abs_top_builddir}/tests:"${PATH}" && export XERCESC_NLS_HOME=${abs_top_builddir}/src/.libs && cd ${top_srcdir} && perl scripts/sanityTest.pl 2>&1 | ${SED} 's/ *[0-9][0-9]*  *ms */{timing removed}/' 1> ${abs_top_builddir}/test-results.log
	diff test-results.log ${top_srcdir}/scripts/sanityTest_ExpectedResult.log
