// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Fjord Launcher - Minecraft Launcher
 *  Copyright (C) 2024 Evan Goode <mail@evangoo.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file incorporates work from OpenJDK, covered by the following copyright and
 * permission notice:
 *
 *         Copyright (c) 1997, 2024, Oracle and/or its affiliates. All rights reserved.
 *         DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 *         This code is free software; you can redistribute it and/or modify it
 *         under the terms of the GNU General Public License version 2 only, as
 *         published by the Free Software Foundation.  Oracle designates this
 *         particular file as subject to the "Classpath" exception as provided
 *         by Oracle in the LICENSE file that accompanied this code.
 *
 *         This code is distributed in the hope that it will be useful, but WITHOUT
 *         ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *         FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *         version 2 for more details (a copy is included in the LICENSE file that
 *         accompanied this code).
 *
 *         You should have received a copy of the GNU General Public License version
 *         2 along with this work; if not, write to the Free Software Foundation,
 *         Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *         Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 *         or visit www.oracle.com if you need additional information or have any
 *         questions.
 *
 * The GNU General Public License version 2 is available at https://www.gnu.org/licenses/old-licenses/gpl-2.0.html.
 */

#include <string.h>
#include <iostream>

#include "Manifest.h"

// Manifest specification:
// https://docs.oracle.com/javase/8/docs/technotes/guides/jar/jar.html#Manifest_Specification

// The implementation of the Manifest class was ported to C++ from OpenJDK:
// https://github.com/openjdk/jdk/blob/0584af23255b6b8f49190eaf2618f3bcc299adfe/src/java.base/share/classes/java/util/jar/Manifest.java#L274

Manifest::Manifest(std::istream& is)
{
    read(is, std::nullopt);
}

Manifest::Manifest(std::istream& is, const std::string& jar_filename)
{
    read(is, jar_filename);
}

bool Manifest::isValidName(const std::string& name)
{
    const auto len = name.length();
    return 0 < len && len <= 70;
}

std::string Manifest::getErrorPosition(const std::optional<std::string>& filename, int line_number)
{
    if (!filename.has_value()) {
        return "line " + std::to_string(line_number);
    }
    return "manifest of " + *filename + ":" + std::to_string(line_number);
}

int Manifest::readAttributes(manifest_section_t& section,
                             std::istream& is,
                             lbuf_t lbuf,
                             const std::optional<std::string>& filename,
                             int line_number)
{
    std::optional<std::string> name;
    std::string value;

    std::string full_line;

    while (!is.eof() && is.getline(lbuf, MANIFEST_MAX_LINE_LENGTH)) {
        std::size_t len = strlen(lbuf);

        line_number += 1;

        if (len > 0 && lbuf[len - 1] == '\r') {
            len -= 1;
        }
        if (len == 0) {
            break;
        }
        std::size_t i = 0;
        if (lbuf[0] == ' ') {
            // continuation of previous line
            if (!name.has_value()) {
                throw std::runtime_error("misplaced continuation line (" + getErrorPosition(filename, line_number) + ")");
            }
            full_line.append(lbuf + 1, len - 1);

            if (is.peek() == ' ') {
                continue;
            }
            value = full_line;
            full_line.clear();
        } else {
            while (lbuf[i++] != ':') {
                if (i >= len) {
                    throw std::runtime_error("invalid header field (" + getErrorPosition(filename, line_number) + ")");
                }
            }
            if (lbuf[i++] != ' ') {
                throw std::runtime_error("invalid header field (" + getErrorPosition(filename, line_number) + ")");
            }
            name = std::string{ lbuf, i - 2 };
            if (is.peek() == ' ') {
                full_line.clear();
                full_line.append(lbuf + i, len - i);
                continue;
            }
            value = std::string{ lbuf + i, len - i };
        }
        if (!isValidName(*name)) {
            throw std::runtime_error("invalid header field name (" + getErrorPosition(filename, line_number) + ")");
        }
        section[*name] = value;
    }
    if (!is.eof() && is.fail()) {
        throw std::length_error("line too long (" + getErrorPosition(filename, line_number) + ")");
    }
    return line_number;
}

std::optional<std::string> Manifest::parseName(lbuf_t lbuf, std::size_t len)
{
    if (tolower(lbuf[0]) == 'n' && tolower(lbuf[1]) && tolower(lbuf[2]) == 'm' && tolower(lbuf[3]) && lbuf[4] == ':' && lbuf[5] == ' ') {
        return std::string{ lbuf, 6, len - 6 };
    }
    return std::nullopt;
}

void Manifest::read(std::istream& is, const std::optional<std::string>& jar_filename)
{
    // Line buffer
    char lbuf[MANIFEST_MAX_LINE_LENGTH];
    // Read the main attributes for the manifest
    int line_number = readAttributes(m_main_section, is, lbuf, jar_filename, 0);

    std::optional<std::string> name;

    bool skip_empty_lines = true;
    std::optional<std::string> lastline;

    while (!is.eof() && is.getline(lbuf, MANIFEST_MAX_LINE_LENGTH)) {
        std::size_t len = strlen(lbuf);

        line_number += 1;

        if (len > 0 && lbuf[len - 1] == '\r') {
            len -= 1;
        }
        if (len == 0 && skip_empty_lines) {
            continue;
        }
        skip_empty_lines = false;

        if (!name.has_value()) {
            name = parseName(lbuf, len);
            if (!name.has_value()) {
                throw std::runtime_error("invalid manifest format (" + getErrorPosition(jar_filename, line_number) + ")");
            }
            if (is.peek() == ' ') {
                // name is wrapped
                lastline = std::string{ lbuf + 6, len - 6 };
                continue;
            }
        } else {
            // continuation line
            std::string buf{ *lastline };
            buf.append(lbuf + 1, len - 1);
            if (is.peek() == ' ') {
                // name is wrapped
                lastline = buf;
                continue;
            }
            name = buf;
            lastline = std::nullopt;
        }

        manifest_section_t& attr = m_individual_sections[*name];
        line_number = readAttributes(attr, is, lbuf, jar_filename, line_number);

        name = std::nullopt;
        skip_empty_lines = true;
    }
    if (!is.eof() && is.fail()) {
        throw std::length_error("manifest line too long (" + getErrorPosition(jar_filename, line_number) + ")");
    }
}
