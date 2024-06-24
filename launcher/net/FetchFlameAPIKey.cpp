// SPDX-License-Identifier: GPL-3.0-only
/*
 *  PolyMC - Minecraft Launcher
 *  Copyright (C) 2022 Lenny McLennington <lenny@sneed.church>
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

#include "FetchFlameAPIKey.h"
#include <BuildConfig.h>
#include <Json.h>
#include "Application.h"

#include <ui/dialogs/CustomMessageBox.h>
#include <ui/dialogs/ProgressDialog.h>

FetchFlameAPIKey::FetchFlameAPIKey(QObject* parent) : Task{ parent } {}

// Here, we fetch the API key from the files of the official CurseForge app. We
// use the macOS disk image (as opposed to the zipped Linux AppImage) since
// there is only one layer of DEFLATE decompression to get through. We
// range-request the specific ~500KiB zlib block from the archive.org mirror
// that contains the API key.
// See also https://git.sakamoto.pl/domi/curseme/src/commit/388ac991eb57dedd5d1aca45f418deb221d757d1/getToken.sh

const QUrl CURSEFORGE_APP_URL{
    "https://web.archive.org/web/20240520233008if_/https://curseforge.overwolf.com/downloads/curseforge-latest.dmg"
};

// To find these offsets:
// 1. Download the disk image from CURSEFORGE_APP_URL and run
//    dmg2img -V ./curseforge-latest.dmg
// 2. Use a hex editor to find the address of the string "cfCoreApiKey" inside
//    curseforge-latest.img.
// 3. In the output of `dmg2img -V`, find the `in_addr`, the `in_size`, and the
// `  out_size` of the block that contains this address.

const uint32_t IN_ADDR{ 4640617 };
const uint32_t IN_SIZE{ 511977 };
const uint32_t OUT_SIZE{ 1048576 };

void FetchFlameAPIKey::executeTask()
{
    QNetworkRequest req{ CURSEFORGE_APP_URL };
    // Request only a single block of the disk image file
    const auto& rangeHeader = QString("bytes=%1-%2").arg(IN_ADDR).arg(IN_ADDR + IN_SIZE);
    req.setRawHeader("Range", rangeHeader.toUtf8());

    m_reply.reset(APPLICATION->network()->get(req));
    connect(m_reply.get(), &QNetworkReply::downloadProgress, this, &Task::setProgress);
    connect(m_reply.get(), &QNetworkReply::finished, this, &FetchFlameAPIKey::downloadFinished);
    connect(m_reply.get(),
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
            &QNetworkReply::errorOccurred,
#else
            qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error),
#endif
            this, [this](QNetworkReply::NetworkError error) {
                qCritical() << "Network error: " << error;
                emitFailed(m_reply->errorString());
            });

    setStatus(tr("Fetching Curseforge core API key (may take a few seconds)..."));
}

void FetchFlameAPIKey::downloadFinished()
{
    auto res = m_reply->readAll();

    // Prepend expected size header. See https://doc.qt.io/qt-6/qbytearray.html#qUncompress-1
    QByteArray expectedSizeHeader;
    QDataStream expectedSizeHeaderStream{ &expectedSizeHeader, QIODevice::WriteOnly };
    expectedSizeHeaderStream.setByteOrder(QDataStream::BigEndian);
    expectedSizeHeaderStream << OUT_SIZE;

    res.prepend(expectedSizeHeader);

    const auto& block = qUncompress(res);
    if (block.isEmpty()) {
        emitFailed("Couldn't decompress Curseforge app data.");
    }

    const char* precedingString = "\"cfCoreApiKey\":\"";
    const QByteArray preceding{ precedingString };
    const auto& precedingIndex = block.indexOf(preceding);
    if (precedingIndex == -1) {
        emitFailed(QString("Couldn't find string '%1'.").arg(precedingString));
    }

    const auto& startIndex = precedingIndex + preceding.size();
    const auto& finalIndex = block.indexOf(QByteArray{ "\"" }, startIndex);
    if (finalIndex == -1) {
        emitFailed("Couldn't find closing \" for cfCoreApiKey value.");
    }

    const auto& keyByteArray = block.mid(startIndex, finalIndex - startIndex);
    m_result = QString{ keyByteArray };
    qDebug() << "Fetched Flame API key: " << m_result;
    emitSucceeded();
}
