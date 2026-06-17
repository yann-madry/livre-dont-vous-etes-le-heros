#ifndef CONDITION_H
#define CONDITION_H

#include <QJsonObject>
#include <QString>

#include "etatjoueur.h"

/**
 * Un choix peut n'apparaître que si sa condition est remplie :
 *  - Aucune          : le choix est toujours visible ;
 *  - PossederObjet   : le joueur doit avoir l'objet `valeur` ;
 *  - EtrePasseParPage: le joueur doit être déjà passé par la page `idPage`.
 */
class Condition
{
public:
    enum class Type {
        Aucune,
        PossederObjet,
        EtrePasseParPage
    };

    Condition() = default;

    Type type() const;
    QString valeur() const;   // nom de l'objet requis
    int idPage() const;       // id de la page requise

    void setObjetRequis(const QString &objet);
    void setPageRequise(int idPage);
    void reinitialiser();

    /// Vrai si la condition est satisfaite par l'état du joueur.
    bool estRemplie(const EtatJoueur &etat) const;


    QJsonObject versJson() const;
    static Condition depuisJson(const QJsonObject &o);

private:
    Type m_type = Type::Aucune;
    QString m_valeur;
    int m_idPage = -1;
};

#endif // CONDITION_H
