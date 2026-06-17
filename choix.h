#ifndef CHOIX_H
#define CHOIX_H

#include <QJsonObject>
#include <QString>

#include "condition.h"
#include "etatjoueur.h"

/**
 * Porte le texte du lien, la page cible, et des EFFETS appliqués au joueur
 * quand il emprunte ce choix (Niveau C) :
 *   - deltaPV   : variation de points de vie (négatif = fuite/blessure) ;
 *   - deltaXP   : gain d'expérience ;
 *   - objetGagne: objet ajouté à l'inventaire (vide = aucun).
 */
class Choix
{
public:
    Choix() = default;
    Choix(const QString &texte, int pageCible);

    // --- Accès ---
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

    // Le choix doit-il être proposé au joueur dans cet état ?
    bool estVisible(const EtatJoueur &etat) const;

    // Applique les effets du choix à l'état du joueur.
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