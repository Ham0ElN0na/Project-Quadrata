#pragma once
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>   // atomic write — writes to temp then renames

// Safely read a JSON object from a file.
// Returns an empty QJsonObject if the file doesn't exist or is invalid.
inline QJsonObject jsonRead(const QString& path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return QJsonObject();
    QByteArray data = f.readAll();
    f.close();
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject())
        return QJsonObject();
    return doc.object();
}

// Safely write a JSON object to a file using QSaveFile (atomic).
// QSaveFile writes to a temp file then renames — prevents corruption on crash/error.
inline bool jsonWrite(const QString& path, const QJsonObject& obj) {
    QSaveFile f(path);
    if (!f.open(QIODevice::WriteOnly)) return false;
    f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
    return f.commit();   // atomic rename — only replaces file if write succeeded
}
