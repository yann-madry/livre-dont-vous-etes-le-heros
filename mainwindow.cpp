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
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
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

    connect(ui->actionNouveau,        &QAction::triggered, this, &MainWindow::nouveau);
    connect(ui->actionOuvrir,         &QAction::triggered, this, &MainWindow::ouvrir);
    connect(ui->actionEnregistrer,    &QAction::triggered, this, &MainWindow::enregistrer);
    connect(ui->actionEnregistrerSous,&QAction::triggered, this, &MainWindow::enregistrerSous);
    connect(ui->actionQuitter,        &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAPropos,        &QAction::triggered, this, &MainWindow::aPropos);

    connect(ui->actionGras,      &QAction::triggered, this, &MainWindow::basculerGras);
    connect(ui->actionItalique,  &QAction::triggered, this, &MainWindow::basculerItalique);
    connect(ui->actionSouligner, &QAction::triggered, this, &MainWindow::souligner);
    connect(ui->actionCouleur,   &QAction::triggered, this, &MainWindow::choisirCouleur);
    connect(ui->actionSurligner, &QAction::triggered, this, &MainWindow::surligner);
    connect(ui->actionPolice,    &QAction::triggered, this, &MainWindow::choisirPolice);

    connect(ui->actionAligner_a_gauche, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignLeft); });
    connect(ui->actionCentrer,          &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignCenter); });
    connect(ui->actionAligner_a_droite, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignRight); });
    connect(ui->actionJustifi,          &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignJustify); });

    connect(ui->btnNouvellePage,  &QPushButton::clicked, this, &MainWindow::nouvellePage);
    connect(ui->btnSupprimerPage, &QPushButton::clicked, this, &MainWindow::supprimerPage);
    connect(ui->listePages, &QListWidget::currentItemChanged, this, &MainWindow::changerPage);

    connect(ui->editeur, &QTextEdit::textChanged, this, &MainWindow::marquerModifie);
    connect(ui->editeur, &QTextEdit::textChanged, this, &MainWindow::majStatistiques);
    connect(ui->titre,   &QLineEdit::textChanged, this, &MainWindow::marquerModifie);

    m_statut = new QLabel(this);
    statusBar()->addPermanentWidget(m_statut);

    creerMenusSupplementaires();
    definirRaccourcis();

    m_pages.append({ "Page de départ", QString() });
    rafraichirListePages();
    ui->listePages->setCurrentRow(0);
    definirFichierCourant("");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::creerMenusSupplementaires()
{
    // --- Menu Fichier : Imprimer + Exporter (insérés avant "A Propos") ---
    QAction *actImprimer = new QAction(QIcon(":/Icones/print.png"), "Imprimer...", this);
    actImprimer->setShortcut(QKeySequence::Print);
    connect(actImprimer, &QAction::triggered, this, &MainWindow::imprimer);
    ui->menuFichier->insertAction(ui->actionAPropos, actImprimer);

    QAction *actExport = new QAction(QIcon::fromTheme("document-export"), "Exporter en page web...", this);
    connect(actExport, &QAction::triggered, this, &MainWindow::exporterHtml);
    ui->menuFichier->insertAction(ui->actionAPropos, actExport);
    ui->menuFichier->insertSeparator(ui->actionAPropos);

    // --- Menu Édition : annuler / rétablir / couper / copier / coller / tout ---
    ui->menuEdition->addSeparator();
    QAction *annuler = ui->menuEdition->addAction(QIcon(":/Icones/edit_undo.png"), "Annuler");
    QAction *retablir = ui->menuEdition->addAction(QIcon(":/Icones/edit_redo.png"), "Rétablir");
    QAction *couper = ui->menuEdition->addAction(QIcon(":/Icones/cut.png"), "Couper");
    QAction *copier = ui->menuEdition->addAction(QIcon(":/Icones/copy.png"), "Copier");
    QAction *coller = ui->menuEdition->addAction(QIcon(":/Icones/paste.png"), "Coller");
    QAction *toutSel = ui->menuEdition->addAction("Tout sélectionner");
    connect(annuler,  &QAction::triggered, ui->editeur, &QTextEdit::undo);
    connect(retablir, &QAction::triggered, ui->editeur, &QTextEdit::redo);
    connect(couper,   &QAction::triggered, ui->editeur, &QTextEdit::cut);
    connect(copier,   &QAction::triggered, ui->editeur, &QTextEdit::copy);
    connect(coller,   &QAction::triggered, ui->editeur, &QTextEdit::paste);
    connect(toutSel,  &QAction::triggered, ui->editeur, &QTextEdit::selectAll);
    annuler->setShortcut(QKeySequence::Undo);
    retablir->setShortcut(QKeySequence::Redo);
    couper->setShortcut(QKeySequence::Cut);
    copier->setShortcut(QKeySequence::Copy);
    coller->setShortcut(QKeySequence::Paste);
    toutSel->setShortcut(QKeySequence::SelectAll);

    // Les mêmes dans la barre d'outils.
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(annuler);
    ui->toolBar->addAction(retablir);
    ui->toolBar->addAction(couper);
    ui->toolBar->addAction(copier);
    ui->toolBar->addAction(coller);

    // --- Menu Insertion ---
    QMenu *menuInsertion = menuBar()->addMenu("Insertion");
    QAction *lien = menuInsertion->addAction(QIcon::fromTheme("insert-link"), "Insérer un lien...");
    lien->setShortcut(QKeySequence("Ctrl+L"));
    connect(lien, &QAction::triggered, this, &MainWindow::insererLien);
    QAction *image = menuInsertion->addAction(QIcon::fromTheme("lien"), "Insérer une image...");
    connect(image, &QAction::triggered, this, &MainWindow::insererImage);
    QAction *liste = menuInsertion->addAction(QIcon::fromTheme("liste"), "Liste à puces");
    connect(liste, &QAction::triggered, this, &MainWindow::insererListe);
    QAction *date = menuInsertion->addAction(QIcon::fromTheme("date"), "Insérer la date");
    connect(date, &QAction::triggered, this, [this]() {
        ui->editeur->insertPlainText(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    });

    // --- Menu Outils ---
    QMenu *menuOutils = menuBar()->addMenu("Outils");
    QAction *zoomP = menuOutils->addAction(QIcon::fromTheme("zoom"), "Zoom avant");
    zoomP->setShortcut(QKeySequence::ZoomIn);
    connect(zoomP, &QAction::triggered, this, [this]() { ui->editeur->zoomIn(2); });
    QAction *zoomM = menuOutils->addAction(QIcon::fromTheme("zoom"), "Zoom arrière");
    zoomM->setShortcut(QKeySequence::ZoomOut);
    connect(zoomM, &QAction::triggered, this, [this]() { ui->editeur->zoomOut(2); });
    menuOutils->addSeparator();
    QAction *html = menuOutils->addAction(QIcon::fromTheme("text-html"), "Afficher le code HTML");
    html->setCheckable(true);
    connect(html, &QAction::triggered, this, [this, html]() {
        if (html->isChecked())
            ui->editeur->setPlainText(ui->editeur->toHtml());
        else
            ui->editeur->setHtml(ui->editeur->toPlainText());
    });
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
    ui->actionPolice->setShortcut(QKeySequence("Ctrl+T"));
    ui->actionSurligner->setShortcut(QKeySequence("Ctrl+H"));
}

// ---------------------------------------------------------------------------
// Pages (stockage local)
// ---------------------------------------------------------------------------

void MainWindow::rafraichirListePages()
{
    ui->listePages->blockSignals(true);
    ui->listePages->clear();
    for (int i = 0; i < m_pages.size(); ++i) {
        const QString t = m_pages[i].titre.isEmpty() ? "(sans titre)" : m_pages[i].titre;
        QListWidgetItem *item = new QListWidgetItem(QString("Page %1 — %2").arg(i + 1).arg(t));
        item->setData(Qt::UserRole, i);
        ui->listePages->addItem(item);
    }
    ui->listePages->blockSignals(false);
}

void MainWindow::afficherPage(int index)
{
    if (index < 0 || index >= m_pages.size())
        return;
    m_pageCourante = index;

    ui->titre->blockSignals(true);
    ui->editeur->blockSignals(true);
    ui->titre->setText(m_pages[index].titre);
    ui->editeur->setHtml(m_pages[index].html);
    ui->titre->blockSignals(false);
    ui->editeur->blockSignals(false);

    majStatistiques();
}

void MainWindow::sauvegarderPageCourante()
{
    if (m_pageCourante < 0 || m_pageCourante >= m_pages.size())
        return;
    m_pages[m_pageCourante].titre = ui->titre->text();
    m_pages[m_pageCourante].html = ui->editeur->toHtml();
}

void MainWindow::changerPage(QListWidgetItem *courant, QListWidgetItem *precedent)
{
    if (precedent) {
        int idx = precedent->data(Qt::UserRole).toInt();
        if (idx >= 0 && idx < m_pages.size()) {
            m_pages[idx].titre = ui->titre->text();
            m_pages[idx].html = ui->editeur->toHtml();
        }
    }
    if (courant)
        afficherPage(courant->data(Qt::UserRole).toInt());
}

void MainWindow::nouvellePage()
{
    sauvegarderPageCourante();
    m_pages.append({ "Nouvelle page", QString() });
    rafraichirListePages();
    ui->listePages->setCurrentRow(m_pages.size() - 1);
}

void MainWindow::supprimerPage()
{
    if (m_pageCourante < 0)
        return;
    if (m_pages.size() <= 1) {
        QMessageBox::information(this, "Suppression", "Un livre doit garder au moins une page.");
        return;
    }
    m_pages.removeAt(m_pageCourante);
    m_pageCourante = -1;
    rafraichirListePages();
    ui->listePages->setCurrentRow(0);
}

// ---------------------------------------------------------------------------
// Fichier
// ---------------------------------------------------------------------------

void MainWindow::nouveau()
{
    if (!confirmerAbandonModifs())
        return;
    m_pages.clear();
    m_pageCourante = -1;
    m_pages.append({ "Page de départ", QString() });
    rafraichirListePages();
    ui->listePages->setCurrentRow(0);
    definirFichierCourant("");
}

void MainWindow::ouvrir()
{
    if (!confirmerAbandonModifs())
        return;
    const QString chemin = QFileDialog::getOpenFileName(this, "Ouvrir", QString(),
                                                        "Tous les formats (*.json *.html *.txt);;Livre LDVELH (*.json);;HTML (*.html);;Texte (*.txt)");
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
                                                        "Livre LDVELH (*.json);;Page HTML (*.html);;Texte (*.txt)");
    if (chemin.isEmpty())
        return false;
    return ecrireFichier(chemin);
}

bool MainWindow::ecrireFichier(const QString &chemin)
{
    sauvegarderPageCourante();

    QFile fichier(chemin);
    if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'enregistrer :\n" + chemin);
        return false;
    }
    QTextStream flux(&fichier);

    if (chemin.endsWith(".html", Qt::CaseInsensitive)) {
        // Page courante en HTML brut.
        flux << ui->editeur->toHtml();
    } else if (chemin.endsWith(".txt", Qt::CaseInsensitive)) {
        // Page courante en texte simple.
        flux << ui->editeur->toPlainText();
    } else {
        // Livre complet en JSON (toutes les pages).
        QJsonArray tableau;
        for (const PageDoc &p : m_pages) {
            QJsonObject o;
            o["titre"] = p.titre;
            o["html"]  = p.html;
            tableau.append(o);
        }
        QJsonObject racine;
        racine["pages"] = tableau;
        flux << QString::fromUtf8(QJsonDocument(racine).toJson(QJsonDocument::Indented));
    }
    fichier.close();

    definirFichierCourant(chemin);
    statusBar()->showMessage("Enregistré : " + chemin, 3000);
    return true;
}

void MainWindow::chargerFichier(const QString &chemin)
{
    QFile fichier(chemin);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir :\n" + chemin);
        return;
    }
    const QByteArray donnees = fichier.readAll();
    fichier.close();

    if (chemin.endsWith(".json", Qt::CaseInsensitive)) {
        const QJsonDocument doc = QJsonDocument::fromJson(donnees);
        m_pages.clear();
        const QJsonArray tableau = doc.object()["pages"].toArray();
        for (const QJsonValue &v : tableau) {
            const QJsonObject o = v.toObject();
            m_pages.append({ o["titre"].toString(), o["html"].toString() });
        }
        if (m_pages.isEmpty())
            m_pages.append({ "Page de départ", QString() });
    } else {
        // HTML ou texte : on charge dans une seule page.
        PageDoc p;
        p.titre = QFileInfo(chemin).completeBaseName();
        if (chemin.endsWith(".html", Qt::CaseInsensitive))
            p.html = QString::fromUtf8(donnees);
        else
            p.html = QString::fromUtf8(donnees).toHtmlEscaped();
        m_pages.clear();
        m_pages.append(p);
    }

    m_pageCourante = -1;
    rafraichirListePages();
    ui->listePages->setCurrentRow(0);
    definirFichierCourant(chemin);
    statusBar()->showMessage("Ouvert : " + chemin, 3000);
}

void MainWindow::imprimer()
{
    QPrinter imprimante;
    QPrintDialog dialogue(&imprimante, this);
    if (dialogue.exec() == QDialog::Accepted)
        ui->editeur->print(&imprimante);
}

// ---------------------------------------------------------------------------
// Export en page web (page courante)
// ---------------------------------------------------------------------------

void MainWindow::exporterHtml()
{
    sauvegarderPageCourante();

    const QString chemin = QFileDialog::getSaveFileName(this, "Exporter en page web",
                                                        QString(), "Page web (*.html)");
    if (chemin.isEmpty())
        return;

    const QString titre = ui->titre->text().isEmpty() ? "Mon livre" : ui->titre->text();

    const QString htmlEditeur = ui->editeur->toHtml();
    QString corps = htmlEditeur;
    int debut = htmlEditeur.indexOf("<body");
    if (debut != -1) {
        debut = htmlEditeur.indexOf('>', debut) + 1;
        int fin = htmlEditeur.indexOf("</body>", debut);
        corps = htmlEditeur.mid(debut, fin - debut);
    }

    const QString page = QString(
                             "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n<meta charset=\"UTF-8\">\n"
                             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
                             "<title>%1</title>\n"
                             "<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n"
                             "<link href=\"https://fonts.googleapis.com/css2?family=Cinzel:wght@600;800&"
                             "family=EB+Garamond:ital@0;1&display=swap\" rel=\"stylesheet\">\n"
                             "<style>\n"
                             "  * { box-sizing:border-box; }\n"
                             "  body { margin:0; min-height:100vh; padding:48px 20px;\n"
                             "         background:radial-gradient(circle at 50% 0%, #3a3326, #1d1a14 70%);\n"
                             "         font-family:'EB Garamond', Georgia, serif; color:#33271a; }\n"
                             "  .page { max-width:760px; margin:auto; background:#efe2c4;\n"
                             "          background-image:radial-gradient(rgba(120,90,40,.07) 1px, transparent 1px);\n"
                             "          background-size:13px 13px; padding:56px 60px; border-radius:5px;\n"
                             "          border:1px solid #b89b6e;\n"
                             "          box-shadow:0 0 0 7px rgba(0,0,0,.28), 0 18px 44px rgba(0,0,0,.6);\n"
                             "          position:relative; animation:apparition .6s ease; }\n"
                             "  .page::before { content:''; position:absolute; inset:11px;\n"
                             "          border:1px solid rgba(138,99,52,.4); border-radius:3px; pointer-events:none; }\n"
                             "  @keyframes apparition { from{opacity:0; transform:translateY(12px);} to{opacity:1;} }\n"
                             "  .ornement { text-align:center; color:#a07b46; letter-spacing:6px; font-size:14px; }\n"
                             "  h1.titre { font-family:'Cinzel', serif; font-weight:800; text-align:center;\n"
                             "          color:#5a3d23; font-size:2.2rem; letter-spacing:1px; margin:6px 0 24px;\n"
                             "          text-shadow:1px 1px 0 rgba(255,255,255,.45); }\n"
                             "  .contenu p { line-height:1.85; font-size:1.15rem; text-align:justify; }\n"
                             "  .contenu > p:first-of-type::first-letter { float:left; font-family:'Cinzel', serif;\n"
                             "          font-size:3.6rem; line-height:.8; color:#8b3a1d; padding:6px 10px 0 0; }\n"
                             "  img { max-width:100%; border-radius:4px; display:block; margin:20px auto;\n"
                             "        box-shadow:0 6px 18px rgba(0,0,0,.35); }\n"
                             "  a { display:inline-block; margin:8px; padding:13px 26px;\n"
                             "      background:linear-gradient(#9c6a3b, #7a4f29); color:#fff5e6;\n"
                             "      text-decoration:none; border-radius:8px; font-family:'Cinzel',serif;\n"
                             "      font-weight:600; letter-spacing:.5px; border:1px solid #5a3a1c;\n"
                             "      box-shadow:0 4px 0 #4a3017, 0 6px 12px rgba(0,0,0,.4); transition:all .15s ease; }\n"
                             "  a:hover { transform:translateY(-2px); box-shadow:0 6px 0 #4a3017, 0 10px 18px rgba(0,0,0,.45); }\n"
                             "  a:active { transform:translateY(2px); box-shadow:0 1px 0 #4a3017; }\n"
                             "</style>\n</head>\n<body>\n  <div class=\"page\">\n"
                             "    <div class=\"ornement\">&#10022; &#10022; &#10022;</div>\n"
                             "    <h1 class=\"titre\">%1</h1>\n"
                             "    <div class=\"contenu\">\n%2\n    </div>\n  </div>\n</body>\n</html>\n")
                             .arg(titre, corps);

    QFile fichier(chemin);
    if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'écrire :\n" + chemin);
        return;
    }
    QTextStream flux(&fichier);
    flux << page;
    fichier.close();
    statusBar()->showMessage("Page web exportée : " + chemin, 3000);
}

// ---------------------------------------------------------------------------
// Mise en forme
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Insertion
// ---------------------------------------------------------------------------

void MainWindow::insererLien()
{
    bool ok = false;
    const QString url = QInputDialog::getText(this, "Insérer un lien",
                                              "Page cible (ex. page2.html) ou URL :", QLineEdit::Normal, "", &ok);
    if (!ok || url.isEmpty())
        return;
    QString texte = QInputDialog::getText(this, "Insérer un lien",
                                          "Texte du choix :", QLineEdit::Normal, url, &ok);
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
    ui->editeur->textCursor().insertImage(image);
}

void MainWindow::insererListe()
{
    ui->editeur->textCursor().insertList(QTextListFormat::ListDisc);
}

// ---------------------------------------------------------------------------
// Divers
// ---------------------------------------------------------------------------

void MainWindow::aPropos()
{
    QMessageBox::about(this, "À propos",
                       "Éditeur LDVELH — SAÉ 2.01\nÉditeur de livre dont vous êtes le héros (Qt Widgets).");
}

void MainWindow::marquerModifie()
{
    setWindowModified(true);
}

void MainWindow::majStatistiques()
{
    const QString texte = ui->editeur->toPlainText();
    const int caracteres = texte.length();
    const int mots = texte.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts).count();
    m_statut->setText(QString("Page %1 · %2 mots · %3 caractères")
                          .arg(m_pageCourante + 1).arg(mots).arg(caracteres));
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
    const auto reponse = QMessageBox::warning(this, "Livre modifié",
                                              "Le livre a été modifié.\nVoulez-vous l'enregistrer ?",
                                              QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (reponse == QMessageBox::Save)
        return enregistrer();
    if (reponse == QMessageBox::Cancel)
        return false;
    return true;
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
    ? "Nouveau livre"
    : QFileInfo(m_fichierCourant).fileName();
    setWindowTitle(nom + "[*] - Éditeur LDVELH");
}