#ifndef CONDITION_H
#define CONDITION_H

#include <QJsonObject>
#include <QString>

#include "etatjoueur.h"

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
    QString valeur() const;
    int idPage() const;

    void setObjetRequis(const QString &objet);
    void setPageRequise(int idPage);
    void reinitialiser();

    bool estRemplie(const EtatJoueur &etat) const;

    QJsonObject versJson() const;
    static Condition depuisJson(const QJsonObject &o);

private:
    Type m_type = Type::Aucune;
    QString m_valeur;
    int m_idPage = -1;
};

#endif
