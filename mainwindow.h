#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include "livre.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;
class QListWidgetItem;
class QTimer;
class FenetreRun;

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
    };

    Livre construireLivre() const;

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
};

#endif
