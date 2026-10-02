#ifndef ETATJOUEUR_H
#define ETATJOUEUR_H

#include <QSet>
#include <QString>

struct EtatJoueur
{
    int pv = 0;
    int xp = 0;
    QSet<QString> objets;
    QSet<int> pagesVisitees;

    bool possede(const QString &objet) const { return objets.contains(objet); }
    bool estPassePar(int idPage) const { return pagesVisitees.contains(idPage); }
    bool estMort() const { return pv <= 0; }
};

#endif
