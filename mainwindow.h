#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void nouveau();
    void ouvrir();
    bool enregistrer();
    bool enregistrerSous();
    void choisirPolice();
    void choisirCouleur();
    void basculerGras();
    void basculerItalique();
    void surligner();
    void souligner();
    void aPropos();
    void marquerModifie();
    void majStatistiques();

private:
    void creerMenuEdition();
    void creerMenuOutils();
    void definirRaccourcis();

    bool confirmerAbandonModifs();
    bool ecrireFichier(const QString &chemin);
    void chargerFichier(const QString &chemin);
    void definirFichierCourant(const QString &chemin);
    void majTitre();

    Ui::MainWindow *ui;
    QString m_fichierCourant;
    QLabel *m_statut = nullptr;
};

#endif