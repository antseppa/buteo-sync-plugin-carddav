/*
 * This file is part of buteo-sync-plugin-carddav package
 *
 * Copyright (C) 2014 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Chris Adams <chris.adams@jolla.com>
 *
 * This program/library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program/library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program/library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef SYNCER_P_H
#define SYNCER_P_H

#include "replyparser_p.h"

#include <twowaycontactsyncadapter.h>

#include <QObject>
#include <QDateTime>
#include <QString>
#include <QList>
#include <QPair>
#include <QNetworkAccessManager>

#include <QContactManager>
#include <QContact>

QTCONTACTS_USE_NAMESPACE

class Auth;
class CardDav;
class RequestGenerator;
namespace Buteo { class SyncProfile; }

class Syncer : public QObject, public QtContactsSqliteExtensions::TwoWayContactSyncAdapter
{
    Q_OBJECT

public:
    Syncer(QObject *parent, Buteo::SyncProfile *profile);
   ~Syncer();

    void startSync(int accountId);
    void purgeAccount(int accountId);

Q_SIGNALS:
    void syncSucceeded();
    void syncFailed();

protected:
    // implementing the TWCSA interface
    bool testAccountProvenance(const QContact &contact, const QString &accountId);
    void determineRemoteChanges(const QDateTime &remoteSince,
                                const QString &accountId);
    void upsyncLocalChanges(const QDateTime &localSince,
                            const QList<QContact> &locallyAdded,
                            const QList<QContact> &locallyModified,
                            const QList<QContact> &locallyDeleted,
                            const QString &accountId);

private:
    bool readExtraStateData(int accountId);
    bool storeExtraStateData(int accountId);

private Q_SLOTS:
    void sync(const QString &serverUrl, const QString &username, const QString &password, const QString &accessToken);
    void continueSync(const QList<QContact> &added, const QList<QContact> &modified, const QList<QContact> &removed);
    void syncFinished();
    void signInError();
    void cardDavError(int errorCode = 0);

private:
    friend class CardDav;
    friend class RequestGenerator;
    friend class ReplyParser;
    Buteo::SyncProfile *m_syncProfile;
    CardDav *m_cardDav;
    Auth *m_auth;
    QContactManager m_contactManager;
    QNetworkAccessManager m_qnam;

    // auth related
    int m_accountId;
    QString m_serverUrl;
    QString m_username;
    QString m_password;
    QString m_accessToken;

    // transient
    QString m_defaultAddressbook;
    QMap<QString, QMap<QString, int> > m_serverAdditionIndices;     // uri to index into m_serverAdditions
    QMap<QString, QMap<QString, int> > m_serverModificationIndices; // uti to index into m_serverModifications
    QMap<QString, QList<ReplyParser::ContactInformation> > m_serverAdditions;     // contacts added server-side, per addressbook.
    QMap<QString, QList<ReplyParser::ContactInformation> > m_serverModifications; // contacts modified server-side, per addressbook.
    QMap<QString, QList<ReplyParser::ContactInformation> > m_serverDeletions;     // contacts deleted server-side, per addressbook.

    // loaded from OOB data.
    QMap<QString, QStringList> m_addressbookContactGuids; // addressbookUrl to list of contact guids
    QMap<QString, QString> m_addressbookCtags;
    QMap<QString, QString> m_addressbookSyncTokens;
    QMap<QString, QString> m_contactUids;  // contact guid -> contact UID
    QMap<QString, QString> m_contactUris;  // contact guid -> contact uri
    QMap<QString, QString> m_contactEtags; // contact guid -> contact etag
    QMap<QString, QString> m_contactIds;   // contact guid -> contact id
    QMap<QString, QStringList> m_contactUnsupportedProperties; // contact guid -> prop strings
};

#endif // SYNCER_P_H
