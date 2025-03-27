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

#include "AuthlibInjectorTextureUpload.h"

#include <QHttpMultiPart>

#include "FileSystem.h"
#include "net/ByteArraySink.h"
#include "net/RawHeaderProxy.h"

AuthlibInjectorTextureUpload::AuthlibInjectorTextureUpload(QString path, std::optional<QString> skin_variant) : NetRequest(), m_path(path), m_skin_variant(skin_variant)
{
    logCat = taskMCSkinsLogC;
}

QNetworkReply* AuthlibInjectorTextureUpload::getReply(QNetworkRequest& request)
{
    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    QHttpPart file;
    file.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
    file.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"texture.png\""));
    file.setBody(FS::read(m_path));
    multiPart->append(file);

    if (m_skin_variant.has_value()) {
        QHttpPart model;
        model.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"model\""));
        model.setBody(m_skin_variant->toUtf8());
        multiPart->append(model);
    }

    setStatus(tr("Uploading texture"));
    return m_network->put(request, multiPart);
}

AuthlibInjectorTextureUpload::Ptr AuthlibInjectorTextureUpload::make(MinecraftAccountPtr account, QString path, std::optional<QString> skin_variant)
{
    auto up = makeShared<AuthlibInjectorTextureUpload>(path, skin_variant);
    QString token = account->accessToken();

    QString textureType = skin_variant.has_value() ? "skin" : "cape";
    up->m_url = QUrl(account->accountServerUrl() + "/user/profile/" + account->profileId() + "/" + textureType);
    up->setObjectName(QString("BYTES:") + up->m_url.toString());
    up->m_sink.reset(new Net::ByteArraySink(std::make_shared<QByteArray>()));
    up->addHeaderProxy(new Net::RawHeaderProxy(QList<Net::HeaderPair>{
        { "Authorization", QString("Bearer %1").arg(token).toLocal8Bit() },
    }));
    return up;
}
