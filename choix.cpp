#include "choix.h"

Choix::Choix(const QString &texte, int pageCible)
    : m_texte(texte), m_pageCible(pageCible)
{
}

QString Choix::texte() const
{
    return m_texte;
}

int Choix::pageCible() const
{
    return m_pageCible;
}

int Choix::deltaPV() const
{
    return m_deltaPV;
}

int Choix::deltaXP() const
{
    return m_deltaXP;
}

QString Choix::objetGagne() const
{
    return m_objetGagne;
}

const Condition &Choix::condition() const
{
    return m_condition;
}

Condition &Choix::condition()
{
    return m_condition;
}

void Choix::setTexte(const QString &t)
{
    m_texte = t;
}

void Choix::setPageCible(int id)
{
    m_pageCible = id;
}

void Choix::setDeltaPV(int v)
{
    m_deltaPV = v;
}

void Choix::setDeltaXP(int v)
{
    m_deltaXP = v;
}

void Choix::setObjetGagne(const QString &o)
{
    m_objetGagne = o;
}

void Choix::setCondition(const Condition &c)
{
    m_condition = c;
}

bool Choix::estVisible(const EtatJoueur &etat) const
{
    return m_condition.estRemplie(etat);
}

void Choix::appliquerEffets(EtatJoueur &etat) const
{
    etat.pv += m_deltaPV;
    etat.xp += m_deltaXP;
    if (!m_objetGagne.isEmpty())
        etat.objets.insert(m_objetGagne);
}

QJsonObject Choix::versJson() const
{
    QJsonObject o;
    o["texte"]      = m_texte;
    o["pageCible"]  = m_pageCible;
    o["deltaPV"]    = m_deltaPV;
    o["deltaXP"]    = m_deltaXP;
    o["objetGagne"] = m_objetGagne;
    o["condition"]  = m_condition.versJson();
    return o;
}

Choix Choix::depuisJson(const QJsonObject &o)
{
    Choix c;
    c.m_texte      = o["texte"].toString();
    c.m_pageCible  = o["pageCible"].toInt(-1);
    c.m_deltaPV    = o["deltaPV"].toInt(0);
    c.m_deltaXP    = o["deltaXP"].toInt(0);
    c.m_objetGagne = o["objetGagne"].toString();
    c.m_condition  = Condition::depuisJson(o["condition"].toObject());
    return c;
}