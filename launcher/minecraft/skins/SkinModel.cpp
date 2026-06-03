// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2023-2025 Trial97 <alexandru.tripon97@gmail.com>
 *  Copyright (c) 2025 Rinth, Inc.
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

#include "SkinModel.h"
#include <QFileInfo>
#include <QPainter>

#include "FileSystem.h"

static void setAlpha(QImage& image, const QRect& region, const int alpha)
{
    for (int y = region.top(); y < region.bottom(); ++y) {
        QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = region.left(); x < region.right(); ++x) {
            QRgb pixel = line[x];
            line[x] = qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), alpha);
        }
    }
}

static void doNotchTransparencyHack(QImage& image)
{
    int s = image.width() / 64;
    for (int y = 0; y < 32 * s; y++) {
        QRgb* line = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 32 * s; x < 64 * s; x++) {
            if (qAlpha(line[x]) < 128) {
                return;
            }
        }
    }

    setAlpha(image, { 32 * s, 0, 32 * s, 32 * s }, 0);
}

static QImage improveSkin(QImage skin)
{
    int height = skin.height();
    int width = skin.width();
    if (width < 64 || width % 64 != 0 || (height != width && height != width / 2)) {
        return skin;
    }
    // It seems some older skins may use this format, which can't be drawn onto
    // https://github.com/PrismLauncher/PrismLauncher/issues/4032
    // https://doc.qt.io/qt-6/qpainter.html#begin
    if (skin.format() <= QImage::Format_Indexed8 || !skin.hasAlphaChannel()) {
        skin = skin.convertToFormat(QImage::Format_ARGB32);
    }

    int s = width / 64;
    auto isLegacy = height == width / 2;  // old format
    if (isLegacy) {
        auto newSkin = QImage(QSize(width, width), skin.format());
        newSkin.fill(Qt::transparent);
        QPainter p(&newSkin);
        p.drawImage(0, 0, skin);

        auto copyRect = [&p, &newSkin, s](int startX, int startY, int offsetX, int offsetY, int sizeX, int sizeY) {
            QImage region = newSkin.copy(startX * s, startY * s, sizeX * s, sizeY * s);
            region = region.mirrored(true, false);

            p.drawImage((startX + offsetX) * s, (startY + offsetY) * s, region);
        };
        static const struct {
            int x;
            int y;
            int offsetX;
            int offsetY;
            int width;
            int height;
        } faces[] = {
            { 4, 16, 16, 32, 4, 4 },  { 8, 16, 16, 32, 4, 4 },   { 0, 20, 24, 32, 4, 12 },   { 4, 20, 16, 32, 4, 12 },
            { 8, 20, 8, 32, 4, 12 },  { 12, 20, 16, 32, 4, 12 }, { 44, 16, -8, 32, 4, 4 },   { 48, 16, -8, 32, 4, 4 },
            { 40, 20, 0, 32, 4, 12 }, { 44, 20, -8, 32, 4, 12 }, { 48, 20, -16, 32, 4, 12 }, { 52, 20, -8, 32, 4, 12 },
        };

        for (const auto& face : faces) {
            copyRect(face.x, face.y, face.offsetX, face.offsetY, face.width, face.height);
        }
        doNotchTransparencyHack(newSkin);
        skin = newSkin;
    }
    static const QRect opaqueParts[] = {
        { 0, 0, 32, 16 },
        { 0, 16, 64, 16 },
        { 16, 48, 32, 16 },
    };

    for (const auto& p : opaqueParts) {
        setAlpha(skin, QRect(p.x() * s, p.y() * s, p.width() * s, p.height() * s), 255);
    }
    return skin;
}

static QImage getSkin(const QString path)
{
    return improveSkin(QImage(path));
}

static QImage generatePreviews(QImage texture, bool slim)
{
    QImage preview(36, 36, QImage::Format_ARGB32);
    preview.fill(Qt::transparent);
    QPainter paint(&preview);

    // Scale factor for HD skins
    int s = texture.width() / 64;

    // head
    paint.drawImage(QRect(4, 2, 8, 8), texture.copy(8 * s, 8 * s, 8 * s, 8 * s));
    paint.drawImage(QRect(4, 2, 8, 8), texture.copy(40 * s, 8 * s, 8 * s, 8 * s));
    // torso
    paint.drawImage(QRect(4, 10, 8, 12), texture.copy(20 * s, 20 * s, 8 * s, 12 * s));
    paint.drawImage(QRect(4, 10, 8, 12), texture.copy(20 * s, 36 * s, 8 * s, 12 * s));
    // right leg
    paint.drawImage(QRect(4, 22, 4, 12), texture.copy(4 * s, 20 * s, 4 * s, 12 * s));
    paint.drawImage(QRect(4, 22, 4, 12), texture.copy(4 * s, 36 * s, 4 * s, 12 * s));
    // left leg
    paint.drawImage(QRect(8, 22, 4, 12), texture.copy(20 * s, 52 * s, 4 * s, 12 * s));
    paint.drawImage(QRect(8, 22, 4, 12), texture.copy(4 * s, 52 * s, 4 * s, 12 * s));

    auto armWidth = slim ? 3 : 4;
    auto armPosX = slim ? 1 : 0;
    // right arm
    paint.drawImage(QRect(armPosX, 10, armWidth, 12), texture.copy(44 * s, 20 * s, armWidth * s, 12 * s));
    paint.drawImage(QRect(armPosX, 10, armWidth, 12), texture.copy(44 * s, 36 * s, armWidth * s, 12 * s));
    // left arm
    paint.drawImage(QRect(12, 10, armWidth, 12), texture.copy(36 * s, 52 * s, armWidth * s, 12 * s));
    paint.drawImage(QRect(12, 10, armWidth, 12), texture.copy(52 * s, 52 * s, armWidth * s, 12 * s));

    // back
    // head
    paint.drawImage(QRect(24, 2, 8, 8), texture.copy(24 * s, 8 * s, 8 * s, 8 * s));
    paint.drawImage(QRect(24, 2, 8, 8), texture.copy(56 * s, 8 * s, 8 * s, 8 * s));
    // torso
    paint.drawImage(QRect(24, 10, 8, 12), texture.copy(32 * s, 20 * s, 8 * s, 12 * s));
    paint.drawImage(QRect(24, 10, 8, 12), texture.copy(32 * s, 36 * s, 8 * s, 12 * s));
    // right leg
    paint.drawImage(QRect(24, 22, 4, 12), texture.copy(12 * s, 20 * s, 4 * s, 12 * s));
    paint.drawImage(QRect(24, 22, 4, 12), texture.copy(12 * s, 36 * s, 4 * s, 12 * s));
    // left leg
    paint.drawImage(QRect(28, 22, 4, 12), texture.copy(28 * s, 52 * s, 4 * s, 12 * s));
    paint.drawImage(QRect(28, 22, 4, 12), texture.copy(12 * s, 52 * s, 4 * s, 12 * s));

    // right arm
    paint.drawImage(QRect(armPosX + 20, 10, armWidth, 12), texture.copy((48 + armWidth) * s, 20 * s, armWidth * s, 12 * s));
    paint.drawImage(QRect(armPosX + 20, 10, armWidth, 12), texture.copy((48 + armWidth) * s, 36 * s, armWidth * s, 12 * s));
    // left arm
    paint.drawImage(QRect(32, 10, armWidth, 12), texture.copy((40 + armWidth) * s, 52 * s, armWidth * s, 12 * s));
    paint.drawImage(QRect(32, 10, armWidth, 12), texture.copy((56 + armWidth) * s, 52 * s, armWidth * s, 12 * s));

    return preview;
}
SkinModel::SkinModel(QString path) : m_path(path), m_texture(getSkin(path)), m_model(Model::CLASSIC)
{
    m_preview = generatePreviews(m_texture, false);
}

SkinModel::SkinModel(QDir skinDir, QJsonObject obj)
    : m_capeId(obj["capeId"].toString()), m_model(Model::CLASSIC), m_url(obj["url"].toString())
{
    auto name = obj["name"].toString();

    if (auto model = obj["model"].toString(); model == "SLIM") {
        m_model = Model::SLIM;
    }
    m_path = skinDir.absoluteFilePath(name) + ".png";
    m_texture = getSkin(m_path);
    m_preview = generatePreviews(m_texture, m_model == Model::SLIM);
}

QString SkinModel::name() const
{
    return QFileInfo(m_path).completeBaseName();
}

bool SkinModel::rename(QString newName)
{
    auto info = QFileInfo(m_path);
    auto new_path = FS::PathCombine(info.absolutePath(), newName + ".png");
    if (QFileInfo::exists(new_path)) {
        return false;
    }
    m_path = new_path;
    return FS::move(info.absoluteFilePath(), m_path);
}

QJsonObject SkinModel::toJSON() const
{
    QJsonObject obj;
    obj["name"] = name();
    obj["capeId"] = m_capeId;
    obj["url"] = m_url;
    obj["model"] = getModelString();
    return obj;
}

QString SkinModel::getModelString() const
{
    switch (m_model) {
        case CLASSIC:
            return "CLASSIC";
        case SLIM:
            return "SLIM";
    }
    return {};
}

bool SkinModel::isValid() const
{
    if (m_texture.isNull())
        return false;
    int w = m_texture.width();
    int h = m_texture.height();
    return w >= 64 && w % 64 == 0 && (h == w || h == w / 2);
}
void SkinModel::refresh()
{
    m_texture = getSkin(m_path);
    m_preview = generatePreviews(m_texture, m_model == Model::SLIM);
}
void SkinModel::setModel(Model model)
{
    m_model = model;
    m_preview = generatePreviews(m_texture, m_model == Model::SLIM);
}
