/*
 * Copyright (c) 2021, 2022, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/limits.h>
#include <unistd.h>
#include <libgen.h>
#include "JvmLauncher.h"
#include "LinuxPackage.h"


static char* getModulePath(void) {
    char modulePath[PATH_MAX] = { 0 };
    ssize_t modulePathLen = 0;
    char* result = 0;

    modulePathLen = readlink("/proc/self/exe", modulePath,
                                                    sizeof(modulePath) - 1);
    if (modulePathLen < 0) {
        JP_LOG_ERRNO;
        return 0;
    }
    modulePath[modulePathLen] = '\0';
    result = strdup(modulePath);
    if (!result) {
        JP_LOG_ERRNO;
    }

    return result;
}

static char* concat(const char *x, const char *y) {
    const size_t lenX = strlen(x);
    const size_t lenY = strlen(y);

    char *result = malloc(lenX + lenY + 1 /* \0 */);
    if (!result) {
        JP_LOG_ERRNO;
    } else {
        strcpy(result, x);
        strcat(result, y);
    }

    return result;
}


#define LAUNCHER_LIB_NAME "/libapplauncher.so"

char* getJvmLauncherLibPath(void) {
    char* modulePath = 0;
    char* appImageDir = 0;
    char* launcherLibPath = 0;

    modulePath = getModulePath();
    if (!modulePath) {
        goto cleanup;
    }

    /* Not a package install */
    /* Launcher should be in "bin" subdirectory of app image. */
    /* Launcher lib should be in "lib" subdirectory of app image. */
    appImageDir = dirname(dirname(modulePath));
    launcherLibPath = concat(appImageDir, "/lib" LAUNCHER_LIB_NAME);

cleanup:
    free(modulePath);

    return launcherLibPath;
}
