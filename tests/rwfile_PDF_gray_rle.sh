#!/bin/sh
# Copyright (C) 2004-2009 GraphicsMagick Group
. ${srcdir}/tests/common.shi
${MEMCHECK} ./rwfile  -filespec 'out_gray_rle_%d' -compress rle ${SRCDIR}/input_gray.miff PDF
