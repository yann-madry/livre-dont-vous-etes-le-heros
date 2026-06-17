#ifndef CHOIX_H
#define CHOIX_H

#include <QJsonObject>
#include <QString>

#include "condition.h"
#include "etatjoueur.h"

class Choix
{
public:
    Choix() = default;
    Choix(const QString &texte, int pageCible);

    QString texte() const;
    int pageCible() const;
    int deltaPV() const;
    int deltaXP() const;
    QString objetGagne() const;
    const Condition &condition() const;
    Condition &condition();

    void setTexte(const QString &t);
    void setPageCible(int id);
    void setDeltaPV(int v);
    void setDeltaXP(int v);
    void setObjetGagne(const QString &o);
    void setCondition(const Condition &c);

    bool estVisible(const EtatJoueur &etat) const;

    void appliquerEffets(EtatJoueur &etat) const;

    QJsonObject versJson() const;
    static Choix depuisJson(const QJsonObject &o);

private:
    QString m_texte;
    int m_pageCible = -1;
    int m_deltaPV = 0;
    int m_deltaXP = 0;
    QString m_objetGagne;
    Condition m_condition;
};

#endif
