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
# **************************************************
# SCons_esos.py - Build ESOS EMBEDDED_F14 Applications
# **************************************************
import os
import shutil
Import("env bin2hex linker_side_effect")

# List of copy operations to perform to setup the shared library
# Format: (src, dest)
SHARED_LIB_COPY = [
    ("../../lab_3/include/esos_f14ui.h", "../../lib/include/esos_f14ui.h"),
    ("../../lab_3/src/esos_f14ui.c", "../../lib/src/esos_f14ui.c"),
    ("../../lab_5/include/revF14.h", "../../lib/include/revF14.h"),
    ("../../lab_4/include/esos_pic24_sensor.h", "../../lib/include/esos_pic24_sensor.h"),
    ("../../lab_4/src/esos_pic24_sensor.c", "../../lib/src/esos_pic24_sensor.c"),
    ("../../lab_4/src/esos_sensor.c", "../../lib/src/esos_sensor.c"),
    ("../../lab_5/include/esos_pic24_lcd44780.h", "../../lib/include/esos_pic24_lcd44780.h"),
    ("../../lab_5/src/esos_pic24_lcd44780.c", "../../lib/src/esos_pic24_lcd44780.c"),
    ("../../lab_5/src/esos_lcd44780.c", "../../lib/src/esos_lcd44780.c")
]

SHARED_LIB_NOTE = \
    """/************************************************************
 * WARNING: DO NOT MODIFY THE CONTENTS OF THIS FILE
 * This file is automatically overwritten on each build.
 *
 * To change the contents of this file, copy it to the
 * current lab directory and update SCons_esos.py, or change
 * the file located in: {}.
 ***********************************************************/

"""

# Copy files adding note
for src, dest in SHARED_LIB_COPY:
    if not os.path.exists(os.path.dirname(dest)):
        os.makedirs(os.path.dirname(dest))

    note = SHARED_LIB_NOTE.format(src)
    with open(src, "r") as original:
        data = original.read()
    with open(dest, "w") as modified:
        modified.write(note + data)

PIC24_LIB_FILES = [
    "../../../pic24lib_all/lib/src/pic24_clockfreq.c",
    "../../../pic24lib_all/lib/src/pic24_configbits.c",
    "../../../pic24lib_all/lib/src/pic24_timer.c",
    "../../../pic24lib_all/lib/src/pic24_util.c",
    "../../../pic24lib_all/lib/src/pic24_serial.c",
    "../../../pic24lib_all/lib/src/pic24_uart.c",
    "../../../pic24lib_all/lib/src/pic24_adc.c"]

ESOS_LIB_FILES = ["../../../pic24lib_all/esos/src/esos.c",
                  "../../../pic24lib_all/esos/src/esos_comm.c",
                  "../../../pic24lib_all/esos/src/esos_cb.c",
                  "../../../pic24lib_all/esos/src/esos_mail.c",
                  # "../../../pic24lib_all/esos/src/esos_sensor.c",
                  # "../../../pic24lib_all/esos/src/esos_lcd44780.c",
                  "../../../pic24lib_all/esos/src/pic24/esos_pic24_i2c.c",
                  "../../../pic24lib_all/esos/src/pic24/esos_pic24_irq.c",
                  "../../../pic24lib_all/esos/src/pic24/esos_pic24_rs232.c",
                  "../../../pic24lib_all/esos/src/pic24/esos_pic24_spi.c",
                  "../../../pic24lib_all/esos/src/pic24/esos_pic24_tick.c"]

SHARED_LIB_FILES = Glob("./lib/src/*.c", True, True, True)

# External Libraries
# Combines all pic24/esos files into their own libraries, prevents duplicate env warnings
# this has to go first for some reason
env.StaticLibrary("esos", ESOS_LIB_FILES)
env.StaticLibrary("pic24", PIC24_LIB_FILES)

# Internal Shared Libraries
env.Prepend(CPPPATH=["./lib/include"])
env.StaticLibrary("embedded", SHARED_LIB_FILES)

labDirs = [labDir for labDir in os.listdir(
    "../../") if str.startswith(labDir, "lab_")]
for labDir in labDirs:
    # Enumerate local library files
    INTERNAL_SRC_FILES = Glob(labDir + "/src/*.c", True, True, True)

    # Create env clone for each lab
    labEnv = env.Clone()
    labEnv.Prepend(CPPPATH=[labDir + "/include"])

    # Build each individual application file
    for sourceFile in Glob(labDir + "/*.c", True, True, True):
        prog = labEnv.Program(
            [sourceFile] + INTERNAL_SRC_FILES, LIBS=["esos", "pic24", "embedded"])
        linker_side_effect(labEnv, prog)
        bin2hex(sourceFile, labEnv, "esos")
