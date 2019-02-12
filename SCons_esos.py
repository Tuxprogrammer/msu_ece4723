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
Import("env bin2hex linker_side_effect")

PIC24_LIB_FILES = [
    "../../../pic24lib_all/lib/src/pic24_clockfreq.c",
    "../../../pic24lib_all/lib/src/pic24_configbits.c",
    "../../../pic24lib_all/lib/src/pic24_timer.c",
    "../../../pic24lib_all/lib/src/pic24_util.c",
    "../../../pic24lib_all/lib/src/pic24_serial.c",
    "../../../pic24lib_all/lib/src/pic24_uart.c"]

ESOS_LIB_FILES = [
    "../../../pic24lib_all/esos/src/esos.c",
    "../../../pic24lib_all/esos/src/esos_comm.c",
    "../../../pic24lib_all/esos/src/esos_cb.c",
    "../../../pic24lib_all/esos/src/esos_mail.c",
    "../../../pic24lib_all/esos/src/pic24/esos_pic24_i2c.c",
    "../../../pic24lib_all/esos/src/pic24/esos_pic24_irq.c",
    "../../../pic24lib_all/esos/src/pic24/esos_pic24_rs232.c",
    "../../../pic24lib_all/esos/src/pic24/esos_pic24_spi.c",
    "../../../pic24lib_all/esos/src/pic24/esos_pic24_tick.c"]

# External Libraries
# Combines all pic24/esos files into their own libraries, prevents duplicate env warnings
env.StaticLibrary("esos", ESOS_LIB_FILES)   # this has to go first for some reason
env.StaticLibrary("pic24", PIC24_LIB_FILES)

# Internal Shared Libraries 
SHARED_SRC_FILES = Glob("./src/*.c", True, True, True)
env.Prepend(CPPPATH=["../../include"])
env.StaticLibrary("embedded", SHARED_SRC_FILES)

labDirs = [labDir for labDir in os.listdir("../../") if str.startswith(labDir, "lab_")]
for labDir in labDirs:
    # Enumerate local library files
    INTERNAL_SRC_FILES = Glob(labDir + "/src/*.c", True, True, True)

    # Create env clone for each lab
    labEnv = env.Clone()
    labEnv.Prepend(CPPPATH=[labDir + "/include"])

    # Build each individual application file
    for sourceFile in Glob(labDir + "/*.c", True, True, True):
        prog = labEnv.Program([sourceFile] + INTERNAL_SRC_FILES, LIBS=["esos", "pic24", "embedded"])
        linker_side_effect(labEnv, prog)
        bin2hex(sourceFile, labEnv, "esos")
