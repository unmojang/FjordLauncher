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

package org.prismlauncher.legacy.utils.api;

public class ApiServers {
    public static String getAuthURL() {
        return getPropertyWithFallback("minecraft.api.auth.host", "https://authserver.mojang.com");
    }
    public static String getAccountURL() {
        return getPropertyWithFallback("minecraft.api.account.host", "https://api.mojang.com");
    }
    public static String getSessionURL() {
        return getPropertyWithFallback("minecraft.api.session.host", "https://sessionserver.mojang.com");
    }
    public static String getServicesURL() {
        return getPropertyWithFallback("minecraft.api.services.host", "https://api.minecraftservices.com");
    }
    private static String getPropertyWithFallback(String key, String fallback) {
        String value = System.getProperty(key);
        if (value == null) {
            return fallback;
        }
        return value;
    }
}
