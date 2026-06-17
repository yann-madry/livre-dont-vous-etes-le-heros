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
#include <QLineEdit>
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
#include <QVBoxLayout>
#include <QWidget>
#include <QUrl>
#include <QDesktopServices>
#include <QMouseEvent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionGras->setCheckable(true);
    ui->actionItalique->setCheckable(true);
    ui->actionSouligner->setCheckable(true);



    creerZoneCentrale();

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

    QAction *actExport = ui->menuFichier->addAction("Exporter en page web...");
    connect(actExport, &QAction::triggered, this, &MainWindow::exporterHtml);

    definirIcones();
    creerMenuEdition();
    creerMenuInsertion();
    creerMenuParagraphe();
    creerMenuOutils();
    definirRaccourcis();

    definirFichierCourant("");
    majStatistiques();


    //gestion ouverture lien
    ui->editeur->setTextInteractionFlags(Qt::TextEditorInteraction | Qt::LinksAccessibleByMouse);
    ui->editeur->viewport()->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Place un champ de titre au-dessus de l'éditeur (2 zones).
void MainWindow::creerZoneCentrale()
{
    QWidget *zone = new QWidget(this);
    QVBoxLayout *disposition = new QVBoxLayout(zone);
    disposition->setContentsMargins(0, 0, 0, 0);
    disposition->setSpacing(0);

    m_titre = new QLineEdit(zone);
    m_titre->setPlaceholderText("Titre de la page…");
    m_titre->setStyleSheet(
        "QLineEdit { font-size:22px; font-weight:bold; color:#5a3d23;"
        " padding:12px 16px; border:none; border-bottom:1px solid #ccc;"
        " background:#faf6ee; }");

    disposition->addWidget(m_titre);
    disposition->addWidget(ui->editeur);   // on réutilise l'éditeur du .ui

    setCentralWidget(zone);

    connect(m_titre, &QLineEdit::textChanged, this, &MainWindow::marquerModifie);
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
    QAction *lien = menu->addAction(QIcon(":/Icones/lien.png"), "Insérer un lien...");
    lien->setShortcut(QKeySequence("Ctrl+L"));
    connect(lien, &QAction::triggered, this, &MainWindow::insererLien);
    QAction *image = menu->addAction(QIcon(":/Icones/photo.png"), "Insérer une image...");
    connect(image, &QAction::triggered, this, &MainWindow::insererImage);
    QAction *liste = menu->addAction(QIcon(":/Icones/liste.png"), "Liste à puces");
    connect(liste, &QAction::triggered, this, &MainWindow::insererListe);
    QAction *date = menu->addAction(QIcon(":/Icones/date.png"), "Insérer la date");
    connect(date, &QAction::triggered, this, [this]() {
        ui->editeur->insertPlainText(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    });
}

void MainWindow::creerMenuParagraphe()
{
    QMenu *menu = menuBar()->addMenu("Paragraphe");
    QAction *gauche = menu->addAction(QIcon(":/Icones/gauche.png"), "Aligner à gauche");
    connect(gauche, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignLeft); });
    QAction *centre = menu->addAction(QIcon(":/Icones/centre.png"), "Centrer");
    connect(centre, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignCenter); });
    QAction *droite = menu->addAction(QIcon(":/Icones/droite.png"), "Aligner à droite");
    connect(droite, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignRight); });
    QAction *justifie = menu->addAction(QIcon(":/Icones/justifier.png"), "Justifier");
    connect(justifie, &QAction::triggered, this, [this]() { ui->editeur->setAlignment(Qt::AlignJustify); });
}

void MainWindow::creerMenuOutils()
{
    QMenu *menu = menuBar()->addMenu("Outils");
    QAction *zoomPlus = menu->addAction(QIcon(":/Icones/zoom.png"), "Zoom avant");
    zoomPlus->setShortcut(QKeySequence::ZoomIn);
    connect(zoomPlus, &QAction::triggered, this, [this]() { ui->editeur->zoomIn(2); });
    QAction *zoomMoins = menu->addAction(QIcon(":/Icones/zoom.png"), "Zoom arrière");
    zoomMoins->setShortcut(QKeySequence::ZoomOut);
    connect(zoomMoins, &QAction::triggered, this, [this]() { ui->editeur->zoomOut(2); });
    menu->addSeparator();
    QAction *modeHtml = menu->addAction(QIcon(":/Icones/info.png"), "Afficher le code HTML");
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
    m_titre->clear();
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

void MainWindow::exporterHtml()
{
    const QString chemin = QFileDialog::getSaveFileName(this, "Exporter en page web",
                                                        QString(), "Page web (*.html)");
    if (chemin.isEmpty())
        return;

    const QString titre = m_titre->text().isEmpty() ? "Mon livre" : m_titre->text();

    // Contenu du <body> de l'éditeur.
    const QString htmlEditeur = ui->editeur->toHtml();
    QString corps = htmlEditeur;
    int debut = htmlEditeur.indexOf("<body");
    if (debut != -1) {
        debut = htmlEditeur.indexOf('>', debut) + 1;
        int fin = htmlEditeur.indexOf("</body>", debut);
        corps = htmlEditeur.mid(debut, fin - debut);
    }

    // Page web stylée (Google Fonts + dégradé + carte parchemin + boutons animés).
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
                             "         font-family:'EB Garamond', Georgia, serif; color:#2a2018; }\n"
                             "  .page { max-width:760px; margin:auto; background:#f4ead3;\n"
                             "          background-image:radial-gradient(rgba(120,90,40,.06) 1px, transparent 1px);\n"
                             "          background-size:14px 14px;\n"
                             "          padding:56px 60px; border-radius:6px;\n"
                             "          border:1px solid #b89b6e;\n"
                             "          box-shadow:0 0 0 8px rgba(0,0,0,.25), 0 20px 50px rgba(0,0,0,.6);\n"
                             "          position:relative; animation:apparition .6s ease; }\n"
                             "  .page::before { content:''; position:absolute; inset:14px;\n"
                             "          border:1px solid rgba(138,99,52,.45); border-radius:3px; pointer-events:none; }\n"
                             "  @keyframes apparition { from{opacity:0; transform:translateY(12px);} to{opacity:1;} }\n"
                             "  h1.titre { font-family:'Cinzel', serif; font-weight:800; text-align:center;\n"
                             "          color:#5a3d23; font-size:2.2rem; margin:0 0 6px;\n"
                             "          text-shadow:1px 1px 0 rgba(255,255,255,.4); }\n"
                             "  .ornement { text-align:center; color:#a07b46; letter-spacing:6px; margin-bottom:28px; }\n"
                             "  p { line-height:1.85; font-size:1.15rem; text-align:justify; }\n"
                             "  img { max-width:100%; border-radius:4px; display:block; margin:20px auto;\n"
                             "        box-shadow:0 6px 18px rgba(0,0,0,.35); }\n"
                             "  .choix { text-align:center; margin-top:34px; }\n"
                             "  a { display:inline-block; margin:8px; padding:13px 26px;\n"
                             "      background:linear-gradient(#9c6a3b, #7a4f29); color:#fff5e6;\n"
                             "      text-decoration:none; border-radius:8px; font-family:'Cinzel',serif;\n"
                             "      font-weight:600; letter-spacing:.5px; border:1px solid #5a3a1c;\n"
                             "      box-shadow:0 4px 0 #4a3017, 0 6px 12px rgba(0,0,0,.4);\n"
                             "      transition:all .15s ease; }\n"
                             "  a:hover { transform:translateY(-2px); box-shadow:0 6px 0 #4a3017, 0 10px 18px rgba(0,0,0,.45); }\n"
                             "  a:active { transform:translateY(2px); box-shadow:0 1px 0 #4a3017; }\n"
                             "</style>\n</head>\n<body>\n"
                             "  <div class=\"page\">\n"
                             "    <h1 class=\"titre\">%1</h1>\n"
                             "    <div class=\"ornement\">&#10070; &#10070; &#10070;</div>\n"
                             "%2\n"
                             "  </div>\n</body>\n</html>\n")
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
    // On force un HTML ultra-simple et parfait
    ui->editeur->insertHtml(QString("<a href=\"%1\">%2</a> ").arg(url, texte));
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->editeur->viewport() && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if (mouseEvent->button() == Qt::LeftButton) {
            QString lien = ui->editeur->anchorAt(mouseEvent->pos());

            if (!lien.isEmpty()) {
                QDesktopServices::openUrl(QUrl(lien));
            }
        }
    }

    return QMainWindow::eventFilter(obj, event);
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
    if (chemin.endsWith(".html", Qt::CaseInsensitive)) {
        // On stocke le titre dans une balise commentaire en tête, puis le contenu.
        flux << "<!--titre:" << m_titre->text() << "-->\n";
        flux << ui->editeur->toHtml();
    } else {
        flux << m_titre->text() << "\n\n" << ui->editeur->toPlainText();
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
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le fichier :\n" + chemin);
        return;
    }
    QTextStream flux(&fichier);
    QString contenu = flux.readAll();
    fichier.close();

    QRegularExpression re("^<!--titre:(.*)-->\\n");
    QRegularExpressionMatch m = re.match(contenu);
    if (m.hasMatch()) {
        m_titre->setText(m.captured(1));
        contenu.remove(0, m.capturedLength());
    } else {
        m_titre->clear();
    }

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

