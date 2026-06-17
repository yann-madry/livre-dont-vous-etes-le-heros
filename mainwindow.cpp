#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QIcon>
#include <QImage>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QRegularExpression>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextList>
#include <QTextStream>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionGras->setCheckable(true);
    ui->actionItalique->setCheckable(true);
    ui->actionSouligner->setCheckable(true);

    ui->toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

    connect(ui->actionNouveau,        &QAction::triggered, this, &MainWindow::nouveau);
    connect(ui->actionOuvrir,         &QAction::triggered, this, &MainWindow::ouvrir);
    connect(ui->actionEnregistrer,    &QAction::triggered, this, &MainWindow::enregistrer);
    connect(ui->actionEnregistrerSous,&QAction::triggered, this, &MainWindow::enregistrerSous);
    connect(ui->actionQuitter,        &QAction::triggered, this, &QWidget::close);
    connect(ui->actionGras,           &QAction::triggered, this, &MainWindow::basculerGras);
    connect(ui->actionItalique,       &QAction::triggered, this, &MainWindow::basculerItalique);
    connect(ui->actionSurligner,      &QAction::triggered, this, &MainWindow::surligner);
    connect(ui->actionSouligner,      &QAction::triggered, this, &MainWindow::souligner);
    connect(ui->actionPolice,         &QAction::triggered, this, &MainWindow::choisirPolice);
    connect(ui->actionCouleur,        &QAction::triggered, this, &MainWindow::choisirCouleur);
    connect(ui->actionAPropos,        &QAction::triggered, this, &MainWindow::aPropos);

    connect(ui->editeur, &QTextEdit::textChanged, this, &MainWindow::marquerModifie);
    connect(ui->editeur, &QTextEdit::textChanged, this, &MainWindow::majStatistiques);

    m_statut = new QLabel(this);
    statusBar()->addPermanentWidget(m_statut);

    QAction *actImprimer = ui->menuFichier->addAction(QIcon(":/Icones/print.png"), "Imprimer...");
    actImprimer->setShortcut(QKeySequence::Print);
    connect(actImprimer, &QAction::triggered, this, &MainWindow::imprimer);

    definirIcones();
    creerMenuEdition();
    creerMenuInsertion();
    creerMenuParagraphe();
    creerMenuOutils();
    definirRaccourcis();

    definirFichierCourant("");
    majStatistiques();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::definirIcones()
{
    ui->actionNouveau->setIcon(QIcon(":/Icones/new.png"));
    ui->actionOuvrir->setIcon(QIcon(":/Icones/open.png"));
    ui->actionEnregistrer->setIcon(QIcon(":/Icones/save.png"));
    ui->actionEnregistrerSous->setIcon(QIcon(":/Icones/save_as.png"));
    ui->actionQuitter->setIcon(QIcon(":/Icones/exit.png"));
    ui->actionGras->setIcon(QIcon(":/Icones/bold.png"));
    ui->actionItalique->setIcon(QIcon(":/Icones/italic.png"));
    ui->actionSouligner->setIcon(QIcon(":/Icones/underline.png"));
    ui->actionPolice->setIcon(QIcon(":/Icones/font.png"));
    ui->actionAPropos->setIcon(QIcon(":/Icones/info.png"));
}

void MainWindow::creerMenuEdition()
{
    QAction *annuler = ui->menuEdition->addAction(QIcon(":/Icones/edit_undo.png"), "Annuler");
    connect(annuler, &QAction::triggered, ui->editeur, &QTextEdit::undo);
    QAction *retablir = ui->menuEdition->addAction(QIcon(":/Icones/edit_redo.png"), "Rétablir");
    connect(retablir, &QAction::triggered, ui->editeur, &QTextEdit::redo);
    ui->menuEdition->addSeparator();
    QAction *couper = ui->menuEdition->addAction(QIcon(":/Icones/cut.png"), "Couper");
    connect(couper, &QAction::triggered, ui->editeur, &QTextEdit::cut);
    QAction *copier = ui->menuEdition->addAction(QIcon(":/Icones/copy.png"), "Copier");
    connect(copier, &QAction::triggered, ui->editeur, &QTextEdit::copy);
    QAction *coller = ui->menuEdition->addAction(QIcon(":/Icones/paste.png"), "Coller");
    connect(coller, &QAction::triggered, ui->editeur, &QTextEdit::paste);
    ui->menuEdition->addSeparator();
    QAction *toutSel = ui->menuEdition->addAction("Tout sélectionner");
    connect(toutSel, &QAction::triggered, ui->editeur, &QTextEdit::selectAll);

    annuler->setShortcut(QKeySequence::Undo);
    retablir->setShortcut(QKeySequence::Redo);
    couper->setShortcut(QKeySequence::Cut);
    copier->setShortcut(QKeySequence::Copy);
    coller->setShortcut(QKeySequence::Paste);
    toutSel->setShortcut(QKeySequence::SelectAll);

    ui->toolBar->addSeparator();
    ui->toolBar->addAction(annuler);
    ui->toolBar->addAction(retablir);
    ui->toolBar->addAction(couper);
    ui->toolBar->addAction(copier);
    ui->toolBar->addAction(coller);
}

void MainWindow::creerMenuInsertion()
{
    QMenu *menu = menuBar()->addMenu("Insertion");

    QAction *lien = menu->addAction("Insérer un lien...");
    lien->setShortcut(QKeySequence("Ctrl+L"));
    connect(lien, &QAction::triggered, this, &MainWindow::insererLien);

    QAction *image = menu->addAction("Insérer une image...");
    connect(image, &QAction::triggered, this, &MainWindow::insererImage);

    QAction *liste = menu->addAction("Liste à puces");
    connect(liste, &QAction::triggered, this, &MainWindow::insererListe);

    QAction *date = menu->addAction("Insérer la date");
    connect(date, &QAction::triggered, this, [this]() {
        ui->editeur->insertPlainText(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    });
}

void MainWindow::creerMenuParagraphe()
{
    QMenu *menu = menuBar()->addMenu("Paragraphe");
    QAction *gauche = menu->addAction("Aligner à gauche");
    connect(gauche, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignLeft); });
    QAction *centre = menu->addAction("Centrer");
    connect(centre, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignCenter); });
    QAction *droite = menu->addAction("Aligner à droite");
    connect(droite, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignRight); });
    QAction *justifie = menu->addAction("Justifier");
    connect(justifie, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignJustify); });
}

void MainWindow::creerMenuOutils()
{
    QMenu *menu = menuBar()->addMenu("Outils");
    QAction *zoomPlus = menu->addAction("Zoom avant");
    zoomPlus->setShortcut(QKeySequence::ZoomIn);
    connect(zoomPlus, &QAction::triggered, this, [this]() { ui->editeur->zoomIn(2); });
    QAction *zoomMoins = menu->addAction("Zoom arrière");
    zoomMoins->setShortcut(QKeySequence::ZoomOut);
    connect(zoomMoins, &QAction::triggered, this, [this]() { ui->editeur->zoomOut(2); });
    menu->addSeparator();
    QAction *modeHtml = menu->addAction("Afficher le code HTML");
    modeHtml->setCheckable(true);
    connect(modeHtml, &QAction::triggered, this, &MainWindow::basculerModeHtml);
}

void MainWindow::basculerModeHtml()
{
    QAction *action = qobject_cast<QAction*>(sender());
    const bool modeHtml = action && action->isChecked();
    if (modeHtml)
        ui->editeur->setPlainText(ui->editeur->toHtml());
    else
        ui->editeur->setHtml(ui->editeur->toPlainText());
}

void MainWindow::definirRaccourcis()
{
    ui->actionNouveau->setShortcut(QKeySequence::New);
    ui->actionOuvrir->setShortcut(QKeySequence::Open);
    ui->actionEnregistrer->setShortcut(QKeySequence::Save);
    ui->actionEnregistrerSous->setShortcut(QKeySequence::SaveAs);
    ui->actionQuitter->setShortcut(QKeySequence::Quit);
    ui->actionGras->setShortcut(QKeySequence::Bold);
    ui->actionItalique->setShortcut(QKeySequence::Italic);
    ui->actionSouligner->setShortcut(QKeySequence::Underline);
    ui->actionSurligner->setShortcut(QKeySequence("Ctrl+H"));
    ui->actionPolice->setShortcut(QKeySequence("Ctrl+T"));
}

void MainWindow::majStatistiques()
{
    const QString texte = ui->editeur->toPlainText();
    const int caracteres = texte.length();
    const int mots = texte.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();
    m_statut->setText(QString("%1 mots · %2 caractères").arg(mots).arg(caracteres));
}

void MainWindow::nouveau()
{
    if (!confirmerAbandonModifs())
        return;
    ui->editeur->clear();
    definirFichierCourant("");
}

void MainWindow::ouvrir()
{
    if (!confirmerAbandonModifs())
        return;
    const QString chemin = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", QString(),
                                                        "Texte et HTML (*.txt *.html);;Tous les fichiers (*)");
    if (!chemin.isEmpty())
        chargerFichier(chemin);
}

bool MainWindow::enregistrer()
{
    if (m_fichierCourant.isEmpty())
        return enregistrerSous();
    return ecrireFichier(m_fichierCourant);
}

bool MainWindow::enregistrerSous()
{
    const QString chemin = QFileDialog::getSaveFileName(this, "Enregistrer sous", QString(),
                                                        "Texte (*.txt);;HTML (*.html)");
    if (chemin.isEmpty())
        return false;
    return ecrireFichier(chemin);
}

void MainWindow::imprimer()
{
    QPrinter imprimante;
    QPrintDialog dialogue(&imprimante, this);
    if (dialogue.exec() == QDialog::Accepted)
        ui->editeur->print(&imprimante);
}

void MainWindow::choisirPolice()
{
    bool ok = false;
    QFont police = QFontDialog::getFont(&ok, ui->editeur->currentFont(), this);
    if (ok)
        ui->editeur->setCurrentFont(police);
}

void MainWindow::choisirCouleur()
{
    QColor couleur = QColorDialog::getColor(ui->editeur->textColor(), this, "Couleur du texte");
    if (couleur.isValid())
        ui->editeur->setTextColor(couleur);
}

void MainWindow::basculerGras()
{
    ui->editeur->setFontWeight(ui->actionGras->isChecked() ? QFont::Bold : QFont::Normal);
}

void MainWindow::basculerItalique()
{
    ui->editeur->setFontItalic(ui->actionItalique->isChecked());
}

void MainWindow::surligner()
{
    QColor couleur = QColorDialog::getColor(Qt::yellow, this, "Couleur de surlignage");
    if (couleur.isValid())
        ui->editeur->setTextBackgroundColor(couleur);
}

void MainWindow::souligner()
{
    ui->editeur->setFontUnderline(ui->actionSouligner->isChecked());
}

void MainWindow::insererLien()
{
    bool ok = false;
    const QString url = QInputDialog::getText(this, "Insérer un lien",
                                              "Adresse (URL ou page) :", QLineEdit::Normal, "", &ok);
    if (!ok || url.isEmpty())
        return;
    QString texte = QInputDialog::getText(this, "Insérer un lien",
                                          "Texte affiché :", QLineEdit::Normal, url, &ok);
    if (!ok)
        return;
    if (texte.isEmpty())
        texte = url;
    ui->editeur->insertHtml(QString("<a href=\"%1\">%2</a> ").arg(url, texte));
}

void MainWindow::insererImage()
{
    const QString chemin = QFileDialog::getOpenFileName(this, "Insérer une image", QString(),
                                                        "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    if (chemin.isEmpty())
        return;

    QImage image(chemin);
    if (image.isNull()) {
        QMessageBox::warning(this, "Erreur", "Image illisible :\n" + chemin);
        return;
    }
    // insertImage intègre l'image directement dans le document.
    ui->editeur->textCursor().insertImage(image);
}

void MainWindow::insererListe()
{
    // Transforme le paragraphe courant en liste à puces.
    ui->editeur->textCursor().insertList(QTextListFormat::ListDisc);
}

void MainWindow::aPropos()
{
    QMessageBox::about(this, "À propos",
                       "Éditeur LDVELH — SAÉ 2.01\nBase 'Notepad' (Niveaux A & B) en Qt Widgets.");
}

void MainWindow::marquerModifie()
{
    setWindowModified(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (confirmerAbandonModifs())
        event->accept();
    else
        event->ignore();
}

bool MainWindow::confirmerAbandonModifs()
{
    if (!isWindowModified())
        return true;
    const auto reponse = QMessageBox::warning(this, "Document modifié",
                                              "Le document a été modifié.\nVoulez-vous enregistrer les changements ?",
                                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (reponse == QMessageBox::Save)
        return enregistrer();
    if (reponse == QMessageBox::Cancel)
        return false;
    return true;
}

bool MainWindow::ecrireFichier(const QString &chemin)
{
    QFile fichier(chemin);
    if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'écrire le fichier :\n" + chemin);
        return false;
    }
    QTextStream flux(&fichier);
    if (chemin.endsWith(".html", Qt::CaseInsensitive))
        flux << ui->editeur->toHtml();
    else
        flux << ui->editeur->toPlainText();
    fichier.close();
    definirFichierCourant(chemin);
    statusBar()->showMessage("Enregistré : " + chemin, 3000);
    return true;
}

void MainWindow::chargerFichier(const QString &chemin)
{
    QFile fichier(chemin);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le fichier :\n" + chemin);
        return;
    }
    QTextStream flux(&fichier);
    const QString contenu = flux.readAll();
    fichier.close();
    if (chemin.endsWith(".html", Qt::CaseInsensitive))
        ui->editeur->setHtml(contenu);
    else
        ui->editeur->setPlainText(contenu);
    definirFichierCourant(chemin);
    statusBar()->showMessage("Ouvert : " + chemin, 3000);
}

void MainWindow::definirFichierCourant(const QString &chemin)
{
    m_fichierCourant = chemin;
    setWindowModified(false);
    majTitre();
}

void MainWindow::majTitre()
{
    const QString nom = m_fichierCourant.isEmpty()
    ? "Sans nom"
    : QFileInfo(m_fichierCourant).fileName();
    setWindowTitle(nom + "[*] - Éditeur LDVELH");
}