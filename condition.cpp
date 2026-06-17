#include "condition.h"

Condition::Type Condition::type() const
{
    return m_type;
}

QString Condition::valeur() const
{
    return m_valeur;
}

int Condition::idPage() const
{
    return m_idPage;
}

void Condition::setObjetRequis(const QString &objet)
{
    m_type = Type::PossederObjet;
    m_valeur = objet;
}

void Condition::setPageRequise(int idPage)
{
    m_type = Type::EtrePasseParPage;
    m_idPage = idPage;
}

void Condition::reinitialiser()
{
    *this = Condition();
}

bool Condition::estRemplie(const EtatJoueur &etat) const
{
    switch (m_type) {
    case Type::Aucune:            return true;
    case Type::PossederObjet:     return etat.possede(m_valeur);
    case Type::EtrePasseParPage:  return etat.estPassePar(m_idPage);
    }
    return true;
}

QJsonObject Condition::versJson() const
{
    QJsonObject o;
    o["type"]   = static_cast<int>(m_type);
    o["valeur"] = m_valeur;
    o["idPage"] = m_idPage;
    return o;
}

Condition Condition::depuisJson(const QJsonObject &o)
{
    Condition c;
    c.m_type   = static_cast<Type>(o["type"].toInt());
    c.m_valeur = o["valeur"].toString();
    c.m_idPage = o["idPage"].toInt(-1);
    return c;
}