#ifndef FENETRERUN_H
#define FENETRERUN_H

#include <QWidget>
#include "livre.h"
#include "etatjoueur.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FenetreRun; }
QT_END_NAMESPACE

class FenetreRun : public QWidget
{
    Q_OBJECT

public:
    explicit FenetreRun(QWidget *parent = nullptr);
    ~FenetreRun() override;

    void chargerLivre(const Livre &livre);

private slots:
    void recommencer();

private:
    void afficherPage(int id);
    void allerVersPage(const Choix &choix);
    void majEtat();
    void viderChoix();
    void afficherFin(const QString &message);

    Ui::FenetreRun *ui;
    Livre m_livre;
    EtatJoueur m_etat;
    int m_pageCourante;
};

#endif
