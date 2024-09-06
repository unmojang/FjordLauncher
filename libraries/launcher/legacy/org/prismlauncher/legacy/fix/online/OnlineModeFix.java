// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 TheKodeToad <TheKodeToad@proton.me>
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
 *  Linking this library statically or dynamically with other modules is
 *  making a combined work based on this library. Thus, the terms and
 *  conditions of the GNU General Public License cover the whole
 *  combination.
 *
 *  As a special exception, the copyright holders of this library give
 *  you permission to link this library with independent modules to
 *  produce an executable, regardless of the license terms of these
 *  independent modules, and to copy and distribute the resulting
 *  executable under terms of your choice, provided that you also meet,
 *  for each linked independent module, the terms and conditions of the
 *  license of that module. An independent module is a module which is
 *  not derived from or based on this library. If you modify this
 *  library, you may extend this exception to your version of the
 *  library, but you are not obliged to do so. If you do not wish to do
 *  so, delete this exception statement from your version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

package org.prismlauncher.legacy.fix.online;

import org.prismlauncher.legacy.utils.api.ApiServers;
import org.prismlauncher.legacy.utils.api.MojangApi;
import org.prismlauncher.legacy.utils.url.ByteArrayUrlConnection;
import org.prismlauncher.legacy.utils.url.UrlUtils;

import java.io.IOException;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.Proxy;
import java.net.URL;
import java.net.URLConnection;
import java.util.HashMap;
import java.util.Map;

public final class OnlineModeFix {
    public static URLConnection openConnection(URL address, Proxy proxy) throws IOException {
        // we start with "http://www.minecraft.net/game/joinserver.jsp?user=..."
        if (!(address.getHost().equals("www.minecraft.net") && address.getPath().equals("/game/joinserver.jsp"))) {
            return null;
        }

        Map<String, String> params = new HashMap<>();
        String query = address.getQuery();
        String[] entries = query.split("&");
        for (String entry : entries) {
            String[] pair = entry.split("=");
            if (pair.length == 2) {
                params.put(pair[0], pair[1]);
            }
        }

        String user = params.get("user");
        if (user == null) {
            throw new AssertionError("missing user");
        }
        String serverId = params.get("serverId");
        if (serverId == null) {
            throw new AssertionError("missing serverId");
        }
        String sessionId = params.get("sessionId");
        if (sessionId == null) {
            throw new AssertionError("missing sessionId");
        }

        // sessionId has the form:
        // token:<accessToken>:<player UUID>
        String accessToken = sessionId.split(":")[1];

        String uuid = null;
        uuid = MojangApi.getUuid(user, proxy);
        if (uuid == null) {
            return new ByteArrayUrlConnection(("Couldn't find UUID of " + user).getBytes("utf-8"));
        }

        URL url = new URL(ApiServers.getSessionURL() + "/session/minecraft/join");
        HttpURLConnection connection = (HttpURLConnection) UrlUtils.openConnection(url, proxy);
        connection.setDoOutput(true);
        connection.setRequestMethod("POST");
        connection.setRequestProperty("Content-Type", "application/json");
        connection.setRequestProperty("Accept", "application/json");
        try (OutputStream os = connection.getOutputStream()) {
            String payload = "{"
                    + "\"accessToken\": \"" + accessToken + "\","
                    + "\"selectedProfile\": \"" + uuid + "\","
                    + "\"serverId\": \"" + serverId + "\""
                    + "}";
            os.write(payload.getBytes("utf-8"));
        }
        int responseCode = connection.getResponseCode();

        if (responseCode == 204) {
            return new ByteArrayUrlConnection("OK".getBytes("utf-8"));
        } else {
            return new ByteArrayUrlConnection("Bad login".getBytes("utf-8"));
        }
    }
}
