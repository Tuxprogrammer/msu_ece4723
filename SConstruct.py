# .. "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
#    All rights reserved.
#    (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
#    (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
#    (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
#
#    Permission to use, copy, modify, and distribute this software and its
#    documentation for any purpose, without fee, and without written agreement is
#    hereby granted, provided that the above copyright notice, the following
#    two paragraphs and the authors appear in all copies of this software.
#
#    IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
#    DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
#    OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
#    HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#    THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
#    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
#    AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
#    ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
#    PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
#
#    Please maintain this header in its entirety when copying/modifying
#    these files.
#
# ****************************************************************
# SConstruct.py - Build all libraries and examples over many chips
# ****************************************************************
# This file contains all that a SConstruct file normally contains.
# In order to use CodeChat, however, all Python files must end with
# a ``.py`` extension. So, SConstuct simply executes this file.
#
#  This file provides an automated build process for the
#  \ref index "libraries" included in this collection.
#  To use:
#
#  #. Install SCons.
#  #. Install the Microchip compiler. Make sure your path
#     includes the directories in which the compiler binaries
#     exist.
#  #. From the command line, change to the directory in which
#     this file lies.
#  #. Execute ``SCons``, which builds everything. Optionally use
#     :doc:`runscons.bat <runscons.bat>` to filter through the resulting
#     warnings.
#
#  The build process can be modified by passing options to
#  SCons. See ``SCons --help`` for options specific
#  to this build and ``SCons -H`` for generic SCons
#  options.
#
# The generated build targets follow the naming convention:
#
#    [bootloader/esos]_hardware platform_mcu_[clock/nofloat]
#
# This module perfoms builds over a variety of CPUs and configurations.
# The files listed below specify a set of targets used in each of these builds.
#
# .. toctree::
#    :maxdepth: 3
#
#    SCons_bootloader.py
#    SCons_build.py
#    SCons_esos.py
#    SCons_zipit.py
#    templates/SConscript.py
#    SConstruct
#
# The overall structure of this file is:
#
# .. contents::

import os
import psutil


# Create a Microchip XC16 Construction Environment
# ================================================
# Define command-line options to set bootloader.
# The Environment below depends on opts, so this must go here instead of with
# the rest of the `Command-line options`_.
opts = Variables()
opts.Add(EnumVariable('BOOTLDR', 'Determines bootloader type', 'msu',
                    allowed_values=('msu', 'none')))

# Create the environment.
env = Environment(
        # Force SCons to set up with gnu tools to start
        # with reasonable defaults. Note: using platform = 'posix'
        # causes SCons to try to call fork() when executing programs
        # (such as compilers), which errors out on Windows.
        tools = ['gcc', 'gnulink', 'ar', 'zip', 'packaging'],
        options = opts,
        CPPPATH = '../../../pic24lib_all/lib/include',
        CC = 'xc16-gcc',
        LIBPATH = '.',
        AR = 'xc16-ar',
        LINK = 'xc16-gcc',
        # Copied and cobbled together from SCons\Tools\cc.py with mods
        CCCOM = '$CC -c -o $TARGET $CFLAGS $CCFLAGS $CPPFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS $SOURCES',
        CCCCOMSTR = 'Compiling $SOURCES',
        # The warnings provide some lint-like checking. Omitted options: -Wstrict-prototypes -Wold-style-definition complains about void foo(), which should be void foo(void), but isn't worth the work to change.
        CCFLAGS = '-mcpu=${MCU} -O1 -msmart-io=1 -omf=elf -Wall -Wextra -Wdeclaration-after-statement -Wlong-long -fdiagnostics-show-option',
        LINKFLAGS = '-mcpu=${MCU} -omf=elf -Wl,--heap=100,--script="$LINKERSCRIPT",--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io',
        LINKERSCRIPT = '../../../pic24lib_all/lib/lkr/p${MCU}_bootldr.gld',
        ARFLAGS = 'rcs',
        ARSTR = 'Create static library: $TARGET',
        OBJSUFFIX = '.o',
        PROGSUFFIX = '.elf',
        # Copy the host envrionment variables for our scons environment
        # so scons can find the build tools and related env vars.
        ENV = os.environ,
      )
#
# Create a bin2hex builder
# ------------------------
# Add the bin2hex function to the environment as a new builder
# This functions converts a binary (.elf or .cof) file to a hex file.
def bin2hex(
  # The name of the .elf/.cof file to be converted.
  binName,
  # An Environment in which to build these sources.
  buildEnvironment,
  # A string to serve as an alias for this build.
  aliasString):

  f = os.path.splitext(binName)[0]
  myHex = buildEnvironment.Hex(f, f)
  # Add this hex file to a convenient alias
  buildEnvironment.Alias(aliasString, myHex)

b2h = Builder(
        action = 'xc16-bin2hex $SOURCE -a -omf=elf',
        suffix = 'hex',
        src_suffix = 'elf')
env.Append(BUILDERS = {'Hex' : b2h})

#
# Linker dependencies
# -------------------
def linker_side_effect(env, program):
  # The linker as of xc16 v1.30 uses a preprocessor by default on linker scripts, which creates a temp file named linker_script.00 (where linker_script is the name of the linker script), which causes parallel builds to fail. This can be disabled via ``--no-cpp``, but then the link fails due to the presence of preprocessor directorive. So, prevent parallel links of the same-named linker script. See http://scons.org/faq.html#How_do_I_prevent_commands_from_being_executed_in_parallel.3F, https://bitbucket.org/scons/scons/wiki/SConsMethods/SideEffect, ``SideEffect`` in http://scons.org/doc/2.5.1/HTML/scons-man.html.
  #
  # Note that the raw ``be['LINKERSCRIPT']`` string contains un-expanded variables, which makes scons unhappy. For example, providing ``bootloader/pic24_dspic33_bootloader.X/lkr/p${MCU}.gld.00`` as a SideEffect the produces errors like ``Internal Error: no cycle found for node build\esos_microstick2_33EP128GP502\chap14\app_ds1722.elf (<SCons.Node.FS.File object at 0x04A2BC60>) in state pending``. So, produce a full, valid file name using subst.
  linker_temp_file = '/' + env.subst(os.path.basename(env['LINKERSCRIPT'])) + '.00'
  env.SideEffect(linker_temp_file, program)
#
# Command-line options
# --------------------
# adjust our default environment based on user command-line requests
dict = env.Dictionary()
if dict['BOOTLDR'] != 'msu':
    env.Replace(LINKERSCRIPT = 'p${MCU}.gld')

# By default, run number_of_cpus*4 jobs at once. Some results from running on my 8-core PC, gathered from the Total build time returned by the --debug=time scons command-line option:
#
# ==  ==========  ===============  ============
# -j  Time (sec)  Time (hh:mm:ss)  Speedup
# ==  ==========  ===============  ============
# 32   303        0:05:03          11.66006601
# 16   348.7      0:05:49          10.13191855
#  8   510.9      0:08:31           6.915247602
#  4   916        0:15:16           3.8569869
#  2  1777        0:29:37           1.98818233
#  1  3533        0:58:53           1
# ==  ==========  ===============  ============

env.SetOption('num_jobs', psutil.cpu_count()*4)
print("Running with -j %d." % GetOption('num_jobs'))

# generate some command line help for our custom options
Help(opts.GenerateHelpText(env))
Help("""Additional targets:
  template-build: Build all .c/.h files which are produced by templates.
  zipit: Build an archive for distributing end-user library contents.
  bootloader: Build the bootloader binaries only.""")

# A DEBUG STATEMENT to see what the scons build envrionment (env) has defined.
#print(env.Dump())
#
#
# Definition of targets
# =====================
# First, set up for defining targets.
#
# Inform SCons about the dependencies in the template-based files.
# SConscript('templates/SConscript.py', 'env')

# Create a target which zips up library files. Only build it if explicitly requested on the command line.
# if 'zipit' in COMMAND_LINE_TARGETS:
#     zip_file = 'build/pic24_code_examples.zip'
#     hg_dir = 'build/pic24lib_all'
#     env.Command(zip_file, '', [
#       # Clone the repo to create a clean distribution.
#       Delete(hg_dir, must_exist = 0),
#       'hg clone . ' + hg_dir,
#       # Copy over hex files from the build.
#       Copy(hg_dir + '/hex', 'hex'),
#       # Perform zip in clean clone.
#       'scons -C ' + hg_dir + ' -f SCons_zipit.py',
#     ])
#     env.Alias('zipit', zip_file)
#     # Only build this if it's explicitly requested. Since the dependencies of ``zip_fil`` are wrong, force a build using AlwaysBuild.
#     env.AlwaysBuild(zip_file)

# Library builds
# ==============
# Call :doc:`SCons_build.py` with a specific buildTargets value. It create a variant build
# named ``hardware_platform _ MCU _ clock``.
# def buildTargetsSConscript(
#   # A list of library targets to build, as defined in :doc:`SCons_build.py`.
#   buildTargets,
#   # The Environment to use for building. Use ``env.Clone(MCU='blah', CPPDEFINES='blah'``
#   # to choose a MCU, clock, and hardware platform as desired.
#   env,
#   # A string giving the name of the hardware platform.
#   hardware_platform,
#   # The same of the clock chosen, or of some special build option (such as
#   # nofloat).
#   extra_defines = ''):
#   # Build a variant directory name, based on the hardware platform, MCU, and extra defines (if any)
#   vdir = 'build/' + '_'.join([hardware_platform, env['MCU']])
#   if extra_defines:
#     vdir += '_' + extra_defines
#   SConscript('SCons_build.py', exports = 'buildTargets env bin2hex linker_side_effect',
#     variant_dir = vdir)

# Build over various MCUs
# -----------------------
# Build small, non-DMA on the PIC24HJ32GP202
# buildTargetsSConscript(['chap08', 'chap09', 'chap10', 'chap11', 'chap12'],
# env.Clone(MCU='24HJ64GP202'), 'default')

# # Build everything on the PIC24FJ64GA002
# buildTargetsSConscript(['chap08', 'chap09', 'chap10', 'chap11', 'chap12',
#                         'chap15'],
#   env.Clone(MCU='24FJ64GA002'), 'default')

# # Build small, non-DMA on the dsPIC33FJ32GP202
# buildTargetsSConscript(['chap08', 'chap09', 'chap10',                'chap11', 'chap12'],
#   env.Clone(MCU='33FJ64GP202'), 'default')

# # Minimally test the 24F16KA102. It has hardmapped UART pins.
# buildTargetsSConscript(['reset', 'echo'],
#   env.Clone(MCU='24F32KA302', CPPDEFINES='HARDWARE_PLATFORM=HARDMAPPED_UART'), 'hardmappedUART')

# # Build the PIC24HJ64GP502-compatible directories.
# buildTargetsSConscript(['chap11', 'chap13', 'chap15'],
#   env.Clone(MCU='24HJ64GP502'), 'default')

# # Same as above, but for a dsPIC
# buildTargetsSConscript(['chap08', 'chap09', 'chap10', 'chap11', 'chap12',
#                         'chap13', 'chap15'],
#   env.Clone(MCU='33FJ128GP802'), 'default')

# # Same as above, but for the dsPIC33EP128GP502
# buildTargetsSConscript(['chap08', 'chap09', 'chap10', 'chap12',
#                         'chap13', 'chap15'],
#   env.Clone(MCU='33EP128GP502'), 'default')

# # Build some for the PIC24E device
# buildTargetsSConscript(['chap08', 'chap09', 'chap10', 'chap11',  'chap12'],
#   env.Clone(MCU='24EP64GP202'), 'default')

# Build over various hardware platforms
# -------------------------------------
# Same as above, but for the dsPIC33EP128GP502 on a MicroStickII target
# buildTargetsSConscript(['chap08', 'chap09', 'chap10', 'chap12',
#                         'chap13', 'chap15'],
#   env.Clone(MCU='33EP128GP502', CPPDEFINES='HARDWARE_PLATFORM=MICROSTICK2'), 'microstick2')

# # Build some selected chapter applications for the chip used on the Fall 2013 Embedded systems board
# buildTargetsSConscript(['chap08', 'chap09', 'chap13'],
#   env.Clone(MCU='33EP128GP504', CPPDEFINES='HARDWARE_PLATFORM=EMBEDDED_C1'), 'embeddedC1')

# # Build some selected chapter applications for the CAN2 rev.F14 board used in ECE4723 Embedded Systems
# buildTargetsSConscript(['chap08', 'chap09'],
#   env.Clone(MCU='33EP512GP806', CPPDEFINES='HARDWARE_PLATFORM=EMBEDDED_F14'), 'embeddedF14')

# # Build for the explorer board
# buildTargetsSConscript(['explorer'],
#   env.Clone(MCU='24FJ128GA010', CPPDEFINES='HARDWARE_PLATFORM=EXPLORER16_100P'), 'explorer16100p')
# buildTargetsSConscript(['explorer'],
#   env.Clone(MCU='24HJ256GP610', CPPDEFINES='HARDWARE_PLATFORM=EXPLORER16_100P'), 'explorer16100p')

# # Build reset on other supported platforms
# buildTargetsSConscript(['reset'],
#   env.Clone(MCU='24FJ64GA002',  CPPDEFINES='HARDWARE_PLATFORM=STARTER_BOARD_28P'), 'starterboard28p')
# buildTargetsSConscript(['reset'],
#   env.Clone(MCU='33FJ128GP204', CPPDEFINES='HARDWARE_PLATFORM=DANGEROUS_WEB'), 'dangerousweb')

# Build over various clocks
# -------------------------
# Build reset with various clock options on all processors
# for clock in ['SIM_CLOCK', 'FRCPLL_FCY16MHz', 'FRC_FCY4MHz',
#   'PRI_NO_PLL_7372KHzCrystal', 'PRIPLL_8MHzCrystal_16MHzFCY']:
#     buildTargetsSConscript(['reset'],
#       env.Clone(MCU='24FJ64GA002', CPPDEFINES='CLOCK_CONFIG=' + clock), 'default', clock)
#     buildTargetsSConscript(['reset'],
#       env.Clone(MCU='24FJ64GA102', CPPDEFINES='CLOCK_CONFIG=' + clock), 'default', clock)
#     buildTargetsSConscript(['reset'],
#       env.Clone(MCU='24F32KA302', CPPDEFINES=['CLOCK_CONFIG=' + clock, 'HARDWARE_PLATFORM=HARDMAPPED_UART']), 'hardmappedUART', clock)
# for clock in ['SIM_CLOCK', 'PRI_NO_PLL_7372KHzCrystal', 'FRC_FCY3685KHz',
#   'FRCPLL_FCY40MHz', 'PRIPLL_7372KHzCrystal_40MHzFCY', 'PRIPLL_8MHzCrystal_40MHzFCY']:
#     buildTargetsSConscript(['reset'],
#       env.Clone(MCU='24HJ32GP202',  CPPDEFINES='CLOCK_CONFIG=' + clock), 'default', clock)
#     buildTargetsSConscript(['reset'],
#       env.Clone(MCU='33FJ128GP802', CPPDEFINES='CLOCK_CONFIG=' + clock), 'default', clock)

# Misc builds
# -----------
# Do a no-float build of reset
# buildTargetsSConscript(['reset'],
#   env.Clone(MCU='24HJ32GP202',  CPPDEFINES='_NOFLOAT'), 'default', 'nofloat')

# Bootloader builds
# =================
# Call :doc:`SCons_bootloader.py` with a specific Environment. It creates a
# variant build named ``bootloader _ hardware_alias _ MCU``.
# def buildTargetsBootloader(
#   # The build environment to use. Typically ``env``, though a ``env.Clone``
#   # can be used to configure env.
#   env,
#   # The MCU to build the bootloader for.
#   mcu,
#   # The DEFINEd name of the target platform
#   hardware_platform = 'DEFAULT_DESIGN',
#   # The string to use in the target build directory name
#   hardware_alias = 'default'):

#     # Create an environment for building the bootloader:
#     # 1. Define the MCU and the target hardware platform
#     env = env.Clone(MCU = mcu, HW = hardware_alias)
#     # 2. Use the custom bootloader linker script.
#     env.Replace(
#         LINKERSCRIPT = 'bootloader/pic24_dspic33_bootloader.X/lkr/p${MCU}.gld',
#     )
#     env.Append(CPPDEFINES = ['BOOTLOADER', 'HARDWARE_PLATFORM=' + hardware_platform])

#     # Now, invoke a variant build using this environment.
#     SConscript('SCons_bootloader.py', exports = 'env bin2hex linker_side_effect',
#       variant_dir = 'build/bootloader_' + hardware_alias + '_' + mcu)

# Build the bootloader for a variety of common MCUs
# that can have UART in the "default" location, e.g.
# RB10=MCUrx and RB11=MCUtx
# for mcu in (
#     '24FJ32GA002',
#     '24FJ64GA002',
#     '24FJ32GA102',
#     '24FJ64GA102',
#     '24FJ64GB002',
#     '24FJ64GB004',

#     '24HJ12GP202',
#     '24HJ32GP202',
#     '24HJ64GP502',
#     '24HJ128GP502',

#     '24EP64GP202',

#     '33FJ32GP202',
#     '33FJ128GP802',

#     '33EP128GP502',
#     '33EP128GP504',
# ):
#     buildTargetsBootloader(env, mcu)

# Build the bootloader for MCUs with a hardmapped UART.
# for mcu in ('24F32KA302',):
#     buildTargetsBootloader(env, mcu,
#     hardware_platform='HARDMAPPED_UART',
#     hardware_alias='hardmappedUART')

# Build bootloader for MCUs on specific hardware platforms.
# buildTargetsBootloader(env,
#     mcu='33EP128GP504',
#     hardware_platform='EMBEDDED_C1',
#     hardware_alias='embeddedC1')

# buildTargetsBootloader(env,
#     mcu='33EP512GP806',
#     hardware_platform='EMBEDDED_F14',
#     hardware_alias='embeddedF14')

# for mcu in (
#     '24FJ64GB002',
#     '24HJ128GP502',
#     '33EP128GP502',
# ):
#     buildTargetsBootloader(env,
#         mcu,
#         hardware_platform='MICROSTICK2',
#         hardware_alias='microstick2')

# ESOS builds
# ===========
def buildTargetsEsos(env, mcu, hardware_platform = 'DEFAULT_DESIGN', hardware_alias = 'default'):
    # Create an environment for building ESOS.
    env = env.Clone(MCU = mcu)
    env.Append(CPPDEFINES = ['BUILT_ON_ESOS', 'HARDWARE_PLATFORM=' + hardware_platform],
               CPPPATH = ['../../../pic24lib_all/esos/include', '../../../pic24lib_all/esos/include/pic24'])

    # Now, invoke a variant build using this environment.
    SConscript('SCons_esos.py', exports = 'env bin2hex linker_side_effect',
      variant_dir = 'build/esos_' + hardware_alias + '_' + mcu)

# Build ESOS over a variety of chips
# that can have UART in the "default" location, e.g.
# RB10=MCUrx and RB11=MCUtx
# for mcu in (
#             '24HJ64GP202',
#             '24FJ64GA002',
#             '24HJ128GP502',
#             '24EP64GP202',
#             '33FJ64GP202',
#             '33FJ128GP802',
#             '33EP128GP502',
#             '33EP128GP504',
#            ):
#     buildTargetsEsos(env, mcu)

# Build the EMBEDDED_F14 Target Board
buildTargetsEsos(env, mcu='33EP512GP806', hardware_platform='EMBEDDED_F14', hardware_alias='embeddedF14')
