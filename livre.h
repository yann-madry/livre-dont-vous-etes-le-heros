#ifndef LIVRE_H
#define LIVRE_H

#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include "page.h"

class Livre
{
public:
    Livre();

    void ajouterPage(const Page &nouvellePage);
    bool contientPage(int id) const;
    Page& getPage(int id);
    void supprimerPage(int id);

    int getIdPageDepart() const;
    void setIdPageDepart(int id);

    void exporterEnSiteWeb(const QString &cheminDossier);
    void genererIndexCartographique(const QString &cheminDossier);

    QMap<int,Page>& getAllPages();

private :
    Page pageErreur;
    QMap<int, Page> livre;
    int idPageDep;
};

#endif
