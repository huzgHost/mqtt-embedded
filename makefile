SHELL = /bin/sh

.PHONY: clean, mkdir

ifndef srcdir
	srcdir = src
endif

ifndef blddir
	blddir = build/output
endif

ifndef prefix 
	prefix = build/output/bin
endif

ifndef exec_prefix
	exec_prefix = ${prefix}
endif

cc ?= gcc

bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib


$(warning $(srcdir))

SOURCE_FILES_C = $(wildcard $(srcdir)/*.c)
OBJS = $(addsuffix .o, $(basename ${SOURCE_FILES_C}))

HEADERS = $(srcdir)/*.h

$(warning $(SOURCE_FILES_C))
$(warning $(HEADERS))
$(warning $(blddir))

SUB_FILES_C = subDemo
SYNC_SUB = $(addprefix ${prefix}, ${SUB_FILES_C})

PUB_FILES_C = pubDemo
SYNC_PUB = $(addprefix ${prefix}, ${PUB_FILES_C})

MQTT_EMBED_LIB_C = launch-embed-mqtt3c

MAJOR_VERSION = 1
MINOR_VERSION = 0
VERSION = ${MAJOR_VERSION}.${MINOR_VERSION}

EMBED_MQTT_C_TARGET = ${blddir}/lib${MQTT_EMBED_LIB_C}.so.${VERSION}

CCFLAGS_SO = -g -fPIC -Os -Wall -fvisibility=hidden -DLINUX_SO
FLAGS_EXE = -I ${srcdir} -L ${blddir}

LDFLAGS_C = -shared -Wl,-soname,lib$(MQTT_EMBED_LIB_C).so.$(MAJOR_VERSION)

all: build

build: | mkdir ${EMBED_MQTT_C_TARGET} ${SYNC_SUB} ${SYNC_PUB}

clean:
	rm -rf ${blddir}/*

mkdir:
	-mkdir -p ${blddir}/output
	-mkdir -p ${blddir}/output/bin

${EMBED_MQTT_C_TARGET}:
	${cc} ${CCFLAGS_SO} -o $@ ${OBJS} ${LDFLAGS_C}
	ln -s lib$(MQTT_EMBED_MQTT_C).so.${VERSION} ${blddir}/lib$(MQTT_EMBED_LIB_C).so.${MAJOR_VERSION}
	ln -s lib$(MQTT_EMBED_MQTT_C).so.${MAJOR_VERSION} ${blddir}/lib$(MQTT_EMBED_LIB_C).so

${SYNC_SUB}: main.c
	${cc} -o $@ $^ -l${MQTT_EMBED_LIB_C} ${FLAGS_EXE}

${SYNC_PUB}: pub.c
	${cc} -o $@ $^ -l${MQTT_EMBED_LIB_C} ${FLAGS_EXE}
