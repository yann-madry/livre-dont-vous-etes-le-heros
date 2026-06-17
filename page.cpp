#include "page.h"

#include <QJsonArray>
#include <QJsonValue>

Page::Page(int id, const QString &titre)
    : m_id(id), m_titre(titre)
{
}

int Page::id() const
{
    return m_id;
}

QString Page::titre() const
{
    return m_titre;
}

QString Page::texteHtml() const
{
    return m_texteHtml;
}

QString Page::image() const
{
    return m_image;
}

Page::Type Page::type() const
{
    return m_type;
}

const QVector<Choix> &Page::choix() const
{
    return m_choix;
}

QVector<Choix> &Page::choix()
{
    return m_choix;
}

void Page::setId(int id)
{
    m_id = id;
}

void Page::setTitre(const QString &t)
{
    m_titre = t;
}

void Page::setTexteHtml(const QString &html)
{
    m_texteHtml = html;
}

void Page::setImage(const QString &chemin)
{
    m_image = chemin;
}

void Page::setType(Type t)
{
    m_type = t;
}

void Page::ajouterChoix(const Choix &c)
{
    m_choix.append(c);
}

bool Page::estFin() const
{
    return m_type != Type::Normale;
}

QJsonObject Page::versJson() const
{
    QJsonObject o;
    o["id"]    = m_id;
    o["titre"] = m_titre;
    o["texte"] = m_texteHtml;
    o["image"] = m_image;
    o["type"]  = static_cast<int>(m_type);
    QJsonArray arr;
    for (const Choix &c : m_choix)
        arr.append(c.versJson());
    o["choix"] = arr;
    return o;
}

Page Page::depuisJson(const QJsonObject &o)
{
    Page p;
    p.m_id        = o["id"].toInt(-1);
    p.m_titre     = o["titre"].toString();
    p.m_texteHtml = o["texte"].toString();
    p.m_image     = o["image"].toString();
    p.m_type      = static_cast<Type>(o["type"].toInt());
    const QJsonArray arr = o["choix"].toArray();
    for (const QJsonValue &v : arr)
        p.m_choix.append(Choix::depuisJson(v.toObject()));
    return p;
}