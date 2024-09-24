/*
 * Copyright (c) 2014-2024 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "settingsservice.h"
#include <QDebug>

SettingsService::SettingsService(QObject *parent) : QObject(parent), m_settings() {}

SettingsService &SettingsService::instance() {
    static SettingsService instance;
    return instance;
}

QHash<QString, QVariant>* SettingsService::cache(bool clear) {
    static QHash<QString, QVariant> cache;

    if (clear) {
        cache.clear();
    }

    return &cache;
}

QVariant SettingsService::value(const QString &key, const QVariant &defaultValue) const {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;
    if (!cache()->contains(fullKey)) {
        cache()->insert(fullKey, m_settings.value(fullKey, defaultValue));
    }

    return cache()->value(fullKey);
}

void SettingsService::setValue(const QString &key, const QVariant &value) {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;
    cache()->insert(fullKey, value);
    m_settings.setValue(fullKey, value);
}

void SettingsService::remove(const QString &key) {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;
    cache()->remove(fullKey);
    m_settings.remove(fullKey);
}

bool SettingsService::contains(const QString &key) const {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;
    return cache()->contains(fullKey) || m_settings.contains(fullKey);
}

void SettingsService::sync() { m_settings.sync(); }

QStringList SettingsService::allKeys() const { return m_settings.allKeys(); }

void SettingsService::clear() {
    cache(true);
    m_settings.clear();
}

void SettingsService::beginGroup(const QString &prefix) {
    m_group = m_group.isEmpty() ? prefix : m_group + '/' + prefix;
    m_settings.beginGroup(prefix);
}

void SettingsService::endGroup() {
    int lastSeparator = m_group.lastIndexOf('/');
    qDebug() << __func__ << " - 'lastSeparator': " << lastSeparator;

    m_group = lastSeparator != -1 ? m_group.left(lastSeparator) : QString();
    m_settings.endGroup();
}

QString SettingsService::group() const { return m_group; }

QString SettingsService::fileName() const { return m_settings.fileName(); }

void SettingsService::beginWriteArray(const QString& prefix, int size) {
    m_settings.beginWriteArray(prefix, size);
    m_arrayPrefix = m_group.isEmpty() ? prefix : m_group + '/' + prefix;
    m_arrayIndex = -1;
}

void SettingsService::setArrayIndex(int i) {
    m_settings.setArrayIndex(i);
    m_arrayIndex = i;
}

void SettingsService::endArray() {
    m_settings.endArray();
    m_arrayPrefix.clear();
    m_arrayIndex = -1;
}

int SettingsService::beginReadArray(QString prefix) {
    m_arrayPrefix = m_group.isEmpty() ? prefix : m_group + '/' + prefix;
    m_arrayIndex = -1;
    return m_settings.beginReadArray(prefix);
}