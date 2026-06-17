#ifndef ETATJOUEUR_H
#define ETATJOUEUR_H

#include <QSet>
#include <QString>

/**
 * État du lecteur-joueur pendant une partie.
 *
 * Sert à deux choses :
 *  - jouer le livre (mode "Run") : suivre les PV, l'XP, les objets ramassés ;
 *  - évaluer les conditions des choix (avoir un objet, être passé par une page).
 */

struct EtatJoueur
{
    int pv = 0;                  // points de vie courants
    int xp = 0;                  // points d'expérience
    QSet<QString> objets;        // objets possédés
    QSet<int> pagesVisitees;     // ids des pages déjà traversées

    bool possede(const QString &objet) const { return objets.contains(objet); }
    bool estPassePar(int idPage) const { return pagesVisitees.contains(idPage); }
    bool estMort() const { return pv <= 0; }
};

#endif // ETATJOUEUR_H
