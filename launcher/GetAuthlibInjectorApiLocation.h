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

#pragma once

#include "minecraft/auth/MinecraftAccount.h"
#include "net/NetRequest.h"

class GetAuthlibInjectorApiLocation : public Net::NetRequest {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<GetAuthlibInjectorApiLocation>;
    GetAuthlibInjectorApiLocation(QUrl url, MinecraftAccountPtr account, QString username);
    virtual ~GetAuthlibInjectorApiLocation() = default;

    static GetAuthlibInjectorApiLocation::Ptr make(QUrl url, MinecraftAccountPtr account, QString username);

    MinecraftAccountPtr getAccount();

    class Sink : public Net::Sink {
       public:
        Sink(GetAuthlibInjectorApiLocation& outer) : m_outer(outer) {}
        virtual ~Sink() = default;

       public:
        auto init(QNetworkRequest& request) -> Task::State override;
        auto write(QByteArray& data) -> Task::State override;
        auto abort() -> Task::State override;
        auto finalize(QNetworkReply& reply) -> Task::State override;
        auto hasLocalData() -> bool override { return false; }

       private:
        GetAuthlibInjectorApiLocation& m_outer;
    };

   protected slots:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;

   private:
    MinecraftAccountPtr m_account;
    QString m_username;
};
