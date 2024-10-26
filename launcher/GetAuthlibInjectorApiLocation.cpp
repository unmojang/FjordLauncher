// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (C) 2023 Evan Goode <mail@evangoo.de>
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

#include "GetAuthlibInjectorApiLocation.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QStringList>
#include <QUrl>
#include "net/ByteArraySink.h"

#include "Application.h"

GetAuthlibInjectorApiLocation::GetAuthlibInjectorApiLocation(QUrl url, MinecraftAccountPtr account, QString username)
    : NetRequest(), m_account(account), m_username(username)
{
    m_url = url;
    m_sink.reset(new Sink(*this));
}

QNetworkReply* GetAuthlibInjectorApiLocation::getReply(QNetworkRequest& request)
{
    setStatus(tr("Getting authlib-injector server details"));
    return m_network->get(request);
}

GetAuthlibInjectorApiLocation::Ptr GetAuthlibInjectorApiLocation::make(QUrl url, MinecraftAccountPtr account, QString username)
{
    return GetAuthlibInjectorApiLocation::Ptr(new GetAuthlibInjectorApiLocation(url, account, username));
}

auto GetAuthlibInjectorApiLocation::Sink::init(QNetworkRequest& request) -> Task::State
{
    return Task::State::Running;
}

auto GetAuthlibInjectorApiLocation::Sink::write(QByteArray& data) -> Task::State
{
    return Task::State::Running;
}

auto GetAuthlibInjectorApiLocation::Sink::abort() -> Task::State
{
    return Task::State::Failed;
}

auto GetAuthlibInjectorApiLocation::Sink::finalize(QNetworkReply& reply) -> Task::State
{
    QVariant header = reply.rawHeader("X-Authlib-Injector-API-Location");
    QUrl url = m_outer.m_url;
    if (header.isValid()) {
        auto location = header.toString();
        url = url.resolved(location);
    } else {
        qDebug() << "X-Authlib-Injector-API-Location header not found!";
    }

    m_outer.m_account.reset(MinecraftAccount::createFromUsernameAuthlibInjector(m_outer.m_username, url.toString()));
    return Task::State::Succeeded;
}

MinecraftAccountPtr GetAuthlibInjectorApiLocation::getAccount()
{
    return m_account;
}
