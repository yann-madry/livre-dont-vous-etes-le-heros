#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Fichier
    void nouveau();
    void ouvrir();
    bool enregistrer();
    bool enregistrerSous();
    void exporterHtml();
    void imprimer();
    void aPropos();

    // Pages
    void nouvellePage();
    void supprimerPage();
    void changerPage(QListWidgetItem *courant, QListWidgetItem *precedent);

    // Mise en forme
    void choisirPolice();
    void choisirCouleur();
    void basculerGras();
    void basculerItalique();
    void surligner();
    void souligner();

    // Insertion
    void insererLien();
    void insererImage();
    void insererListe();

    void marquerModifie();
    void majStatistiques();

private:
    struct PageDoc {
        QString titre;
        QString html;
    };

    void creerMenusSupplementaires();
    void definirRaccourcis();

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
};

#endif // MAINWINDOW_H