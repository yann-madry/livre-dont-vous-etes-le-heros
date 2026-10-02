#ifndef FENETRERUN_H
#define FENETRERUN_H

#include <QWidget>
#include <QMap>
#include <QList>
#include <QStringList>
#include "livre.h"
#include "etatjoueur.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FenetreRun; }
QT_END_NAMESPACE

struct EffetEntree
{
    int pv = 0;
    int xp = 0;
    QStringList objets;
};

class FenetreRun : public QWidget
{
    Q_OBJECT

public:
    explicit FenetreRun(QWidget *parent = nullptr);
    ~FenetreRun() override;

    void chargerLivre(const Livre &livre, int departPv, int departXp,
                      const QStringList &departObjets, const QMap<int, EffetEntree> &effets);

private slots:
    void recommencer();

private:
    void demarrer();
    void afficherPage(int id);
    void allerVersPage(const Choix &choix);
    void retour();
    void ajouterBoutonRetour();
    void majEtat();
    void viderChoix();
    void afficherFin(const QString &message);

    Ui::FenetreRun *ui;
    Livre m_livre;
    EtatJoueur m_etat;
    int m_pageCourante;

    int m_departPv;
    int m_departXp;
    QStringList m_departObjets;
    QMap<int, EffetEntree> m_effets;
    QList<int> m_historiquePage;
    QList<EtatJoueur> m_historiqueEtat;
};

#endif
