#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile -filespec 'out_truecolor12_%d' ${SRCDIR}/input_truecolor12.dpx EPS
