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
 */

#include <map>
#include <optional>
#include <string>

constexpr const int MAX_LINE_LENGTH = 512;
using lbuf_t = char[MAX_LINE_LENGTH];

using section_t = std::map<std::string, std::string>;
using sections_t = std::map<std::string, section_t>;
using manifest_t = std::pair<section_t, sections_t>;

class Manifest {
   public:
    Manifest(std::istream& is);
    section_t& getMainAttributes() { return m_main_section; }
    sections_t& getEntries() { return m_individual_sections; }
    section_t& getAttributes(std::string& name) { return m_individual_sections.at(name); }

   private:
    static std::string getErrorPosition(const std::optional<std::string>& filename, int line_number);
    static std::optional<std::string> parseName(lbuf_t lbuf, std::size_t len);
    static bool isValidName(const std::string& name);
    int readAttributes(section_t& section, std::istream& is, lbuf_t lbuf, const std::optional<std::string>& jar_filename, int line_number);
    void read(std::istream& is, const std::optional<std::string>& jar_filename);
    section_t m_main_section;
    sections_t m_individual_sections;
};
