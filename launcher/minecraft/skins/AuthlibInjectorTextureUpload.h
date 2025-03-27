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

#pragma once

#include <minecraft/auth/MinecraftAccount.h>
#include "net/NetRequest.h"

class AuthlibInjectorTextureUpload : public Net::NetRequest {
    Q_OBJECT
   public:
    using Ptr = shared_qobject_ptr<AuthlibInjectorTextureUpload>;

    // Note this class takes ownership of the file.
    AuthlibInjectorTextureUpload(QString path, std::optional<QString> skin_variant);
    virtual ~AuthlibInjectorTextureUpload() = default;

    static AuthlibInjectorTextureUpload::Ptr make(MinecraftAccountPtr account, QString path, std::optional<QString> skin_variant);

   protected:
    virtual QNetworkReply* getReply(QNetworkRequest&) override;

   private:
    QString m_path;
    std::optional<QString> m_skin_variant;
};
