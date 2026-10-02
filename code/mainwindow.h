#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QString>
#include "livre.h"
#include "fenetrerun.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;
class QListWidgetItem;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    bool eventFilter(QObject *objet, QEvent *event) override;

private slots:
    void nouveau();
    void ouvrir();
    bool enregistrer();
    bool enregistrerSous();
    void exporterHtml();
    void exporterSite();
    void exporterPdf();
    void imprimer();
    void apercuImpression();
    void sauvegardeAuto();
    void aPropos();
    void verifierCoherenceLivre();

    void nouvellePage();
    void supprimerPage();
    void changerPage(QListWidgetItem *courant, QListWidgetItem *precedent);

    void choisirPolice();
    void choisirCouleur();
    void basculerGras();
    void basculerItalique();
    void surligner();
    void souligner();

    void insererLien();
    void insererImage();
    void insererListe();

    void marquerModifie();
    void majStatistiques();

    void changerOnglet(int index);

private:
    struct PageDoc {
        QString titre;
        QString html;
        int effetPv = 0;
        int effetXp = 0;
        QString effetObjet;
    };

    Livre construireLivre() const;
    QMap<int, EffetEntree> construireEffets() const;
    QString nomFichierSain() const;

    void creerMenusSupplementaires();
    void definirRaccourcis();
    void deposerFichier(const QString &chemin);
    QString corpsDeHtml(const QString &html) const;
    QString gabaritPage(const QString &titre, const QString &corps) const;

    void rafraichirListePages();
    void afficherPage(int index);
    void sauvegarderPageCourante();

    bool confirmerAbandonModifs();
    bool ecrireFichier(const QString &chemin);
    void chargerFichier(const QString &chemin);
    void definirFichierCourant(const QString &chemin);
    void majTitre();

    Ui::MainWindow *ui;
    QLabel *m_statut = nullptr;
    QString m_fichierCourant;

    QList<PageDoc> m_pages;
    int m_pageCourante = -1;
    QTimer *m_minuterie = nullptr;
    FenetreRun *m_vueRun = nullptr;

    int m_departPv = 100;
    int m_departXp = 0;
    QString m_departObjets;
    QString m_nomLivre = "Mon livre";
};

#endif
