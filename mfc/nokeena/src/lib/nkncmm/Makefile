# Makefile

WARNINGS_EXTRA_OVERRIDE = 1
include ${PROD_TREE_ROOT}/src/mk/common.mk

LIB=nkn_cmm
LIB_BUILD_DIR=${CUST_LIB_DIR}/nkncmm

SRCS= \
      nkn_cmm_shm_api.c 	\


CFLAGS += -fPIC
CFLAGS += -D_GNU_SOURCE
CFLAGS += -include nkn_no_alloc.h
CFLAGS += ${BUILD_EXTRA_CFLAGS}

ifeq (${BUILD_GCOV}, 1)
  CFLAGS += -fprofile-arcs -ftest-coverage
endif


ifneq (${PROD_FEATURE_I18N_SUPPORT}, 0)
	GETTEXT_PACKAGE=lib${LIB}
	DEFINES+=-DGETTEXT_DOMAIN=\"${GETTEXT_PACKAGE}\"
	SUBMAKEFILES+=Makefile.i18n
endif

LDADD= ${BUILD_EXTRA_LDADD}


include ${PROD_TREE_ROOT}/src/mk/lib.mk
