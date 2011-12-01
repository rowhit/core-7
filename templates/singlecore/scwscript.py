#! /usr/bin/env python
# encoding: utf-8
#*********************************************************************
# Copyright 2010, Institute of Computer and Network Engineering,
#                 TU-Braunschweig
# All rights reserved
# Any reproduction, use, distribution or disclosure of this program,
# without the express, prior written consent of the authors is 
# strictly prohibited.
#
# University of Technology Braunschweig
# Institute of Computer and Network Engineering
# Hans-Sommer-Str. 66
# 38118 Braunschweig, Germany
#
# ESA SPECIAL LICENSE
#
# This program may be freely used, copied, modified, and redistributed
# by the European Space Agency for the Agency's own requirements.
#
# The program is provided "as is", there is no warranty that
# the program is correct or suitable for any purpose,
# neither implicit nor explicit. The program and the information in it
# contained do not necessarily reflect the policy of the European 
# Space Agency or of TU-Braunschweig.
#*********************************************************************
# Title:      wscript
#
# ScssId:
#
# Origin:     HW-SW SystemC Co-Simulation SoC Validation Platform
#
# Purpose:    this file contains the build system for global tests
#
# Method:     $ ./waf configure; ./waf # to build it on top level
#
# Modified on $Date: 2011-05-19 18:31:12 +0200 (Thu, 19 May 2011) $
#          at $Revision: 429 $
#          by $Author: HWSWSIM $
#
# Principal:  European Space Agency
# Author:     VLSI working group @ IDA @ TUBS
# Maintainer: Rolf Meyer
# Reviewed:
#*********************************************************************
top = '..'

def build(bld):
    ambaLayer = '${conf_sys_lt_at}'
    use       = 'ahbctrl irqmp gptimer apbctrl socwire mmu_cache mctrl utils signalkit common TRAP BOOST ELF SYSTEMC AMBA TLM GREENSOCS '
    if ambaLayer == 'true':
        use  += 'leon3.functlt'
    else:
        use  += 'leon3.functat'
      
    bld(
        target       = '$template.$configuration.platform',
        features     = 'cxx cprogram',
        source       = [ 'sc_main.cpp' ],
        includes     = '.',
        use          = use,
        #ut_param     = ['$template.$configuration.prom', 'irqmp.sparc'],
    )
    ldscript = bld.path.find_resource('prom.ld')
    bld(
        target       = '$template.$configuration.prom',
        features     = 'c cprogram sparc',
        linkflags    = '-Ttext=0 -nostartfiles -nostdlib -T%s -N' % (ldscript.abspath()),
        source       = [ 'prom.S' ],
        includes     = '.',
    )