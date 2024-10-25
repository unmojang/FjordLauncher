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

#include "Application.h"
#include "BuildConfig.h"

CreateAuthlibInjectorAccount::CreateAuthlibInjectorAccount(QUrl url, MinecraftAccountPtr account, QString username)
    : NetRequest(), m_url(url), m_account(account), m_username(username)
{}

QNetworkReply* CreateAuthlibInjectorAccount::getReply(QNetworkRequest& request)
{
    setStatus(tr("Getting authlib-injector server details"));
    return m_network->get(request);
}

CreateAuthlibInjectorAccount::Ptr CreateAuthlibInjectorAccount::make(QUrl url, MinecraftAccountPtr account, QString username)
{
    return CreateAuthlibInjectorAccount::Ptr(new CreateAuthlibInjectorAccount(url, account, username));
}

void CreateAuthlibInjectorAccount::downloadFinished()
{
    if (m_state != State::Failed) {
        QVariant header = m_reply->rawHeader("X-Authlib-Injector-API-Location");
        if (header.isValid()) {
            auto location = header.toString();
            m_url = m_url.resolved(location);
        } else {
            qDebug() << "X-Authlib-Injector-API-Location header not found!";
        }
        m_account.reset(MinecraftAccount::createFromUsernameAuthlibInjector(m_username, m_url.toString()));
        m_state = State::Succeeded;
        emit succeeded();
        return;
    } else {
        qDebug() << m_reply->readAll();
        m_reply.reset();
        emitFailed();
        return;
    }
}

MinecraftAccountPtr CreateAuthlibInjectorAccount::getAccount()
{
    return m_account;
}
