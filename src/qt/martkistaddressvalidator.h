// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2014-2020 The Martkist Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MARTKIST_QT_MARTKISTADDRESSVALIDATOR_H
#define MARTKIST_QT_MARTKISTADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class MartkistAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit MartkistAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Martkist address widget validator, checks for a valid martkist address.
 */
class MartkistAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit MartkistAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // MARTKIST_QT_MARTKISTADDRESSVALIDATOR_H
