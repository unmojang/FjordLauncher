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

#include "CreateAuthlibInjectorAccount.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QStringList>
#include <QUrl>
#include "net/ByteArraySink.h"

#include "Application.h"

CreateAuthlibInjectorAccount::CreateAuthlibInjectorAccount(QUrl url, MinecraftAccountPtr account, QString username)
    : NetRequest(), m_account(account), m_username(username)
{
    m_url = url;
    m_sink.reset(new Sink(*this));
}

QNetworkReply* CreateAuthlibInjectorAccount::getReply(QNetworkRequest& request)
{
    setStatus(tr("Getting authlib-injector server details"));
    return m_network->get(request);
}

CreateAuthlibInjectorAccount::Ptr CreateAuthlibInjectorAccount::make(QUrl url, MinecraftAccountPtr account, QString username)
{
    return CreateAuthlibInjectorAccount::Ptr(new CreateAuthlibInjectorAccount(url, account, username));
}

auto CreateAuthlibInjectorAccount::Sink::init(QNetworkRequest& request) -> Task::State
{
    return Task::State::Running;
}

auto CreateAuthlibInjectorAccount::Sink::write(QByteArray& data) -> Task::State
{
    return Task::State::Running;
}

auto CreateAuthlibInjectorAccount::Sink::abort() -> Task::State
{
    return Task::State::Failed;
}

auto CreateAuthlibInjectorAccount::Sink::finalize(QNetworkReply& reply) -> Task::State
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

MinecraftAccountPtr CreateAuthlibInjectorAccount::getAccount()
{
    return m_account;
}
