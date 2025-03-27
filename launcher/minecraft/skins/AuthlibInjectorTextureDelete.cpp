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
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 */

#include "AuthlibInjectorTextureDelete.h"

#include "net/ByteArraySink.h"
#include "net/RawHeaderProxy.h"

AuthlibInjectorTextureDelete::AuthlibInjectorTextureDelete(QString textureType) : NetRequest(), m_textureType(textureType)
{
    logCat = taskMCSkinsLogC;
}

QNetworkReply* AuthlibInjectorTextureDelete::getReply(QNetworkRequest& request)
{
    setStatus(tr("Deleting texture"));
    return m_network->deleteResource(request);
}

AuthlibInjectorTextureDelete::Ptr AuthlibInjectorTextureDelete::make(MinecraftAccountPtr account, QString textureType)
{
    auto up = makeShared<AuthlibInjectorTextureDelete>(textureType);
    QString token = account->accessToken();
    up->m_url = QUrl(account->accountServerUrl() + "/user/profile/" + account->profileId() + "/" + textureType);
    up->m_sink.reset(new Net::ByteArraySink(std::make_shared<QByteArray>()));
    up->addHeaderProxy(new Net::RawHeaderProxy(QList<Net::HeaderPair>{
        { "Authorization", QString("Bearer %1").arg(token).toLocal8Bit() },
    }));
    return up;
}
