#ifndef PAGE_H
#define PAGE_H

#include <QJsonObject>
#include <QString>
#include <QVector>

#include "choix.h"


class Page
{
public:
    enum class Type {
        Normale,
        Victoire,
        Defaite
    };

    Page() = default;
    explicit Page(int id, const QString &titre = QString());


    int id() const;
    QString titre() const;
    QString texteHtml() const;
    QString image() const;
    Type type() const;
    const QVector<Choix> &choix() const;
    QVector<Choix> &choix();

    void setId(int id);
    void setTitre(const QString &t);
    void setTexteHtml(const QString &html);
    void setImage(const QString &chemin);
    void setType(Type t);

    void ajouterChoix(const Choix &c);
    bool estFin() const;


    QJsonObject versJson() const;
    static Page depuisJson(const QJsonObject &o);

private:
    int m_id = -1;
    QString m_titre;
    QString m_texteHtml;
    QString m_image;
    Type m_type = Type::Normale;
    QVector<Choix> m_choix;
};

#endif