#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fenetrerun.h"
#include "page.h"
#include "choix.h"
#include "condition.h"

#include <QAction>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QIcon>
#include <QImage>
#include <QInputDialog>
#include <QMimeData>
#include <QPrintPreviewDialog>
#include <QTimer>
#include <QUrl>
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
#include <QSpinBox>
#include <QSpinBox>
#include <QStatusBar>
#include <QTabWidget>
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

    connect(ui->actionNouveau,         &QAction::triggered, this, &MainWindow::nouveau);
    connect(ui->actionOuvrir,          &QAction::triggered, this, &MainWindow::ouvrir);
    connect(ui->actionEnregistrer,     &QAction::triggered, this, &MainWindow::enregistrer);
    connect(ui->actionEnregistrerSous, &QAction::triggered, this, &MainWindow::enregistrerSous);
    connect(ui->actionQuitter,         &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAPropos,         &QAction::triggered, this, &MainWindow::aPropos);

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

    connect(ui->titre, &QLineEdit::textChanged, this, [this](const QString &t) {
        if (m_pageCourante >= 0 && m_pageCourante < m_pages.size()) {
            m_pages[m_pageCourante].titre = t;
            QListWidgetItem *it = ui->listePages->item(m_pageCourante);
            if (it != nullptr) {
                QString libelle;
                if (t.isEmpty())
                    libelle = "(sans titre)";
                else
                    libelle = t;
                it->setText("Page " + QString::number(m_pageCourante + 1) + " — " + libelle
                            + "  (page" + QString::number(m_pageCourante + 1) + ".html)");
            }
        }
    });

    connect(ui->spinPv, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        if (m_pageCourante >= 0 && m_pageCourante < m_pages.size()) {
            m_pages[m_pageCourante].effetPv = v;
            marquerModifie();
        }
    });
    connect(ui->spinXp, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int v) {
        if (m_pageCourante >= 0 && m_pageCourante < m_pages.size()) {
            m_pages[m_pageCourante].effetXp = v;
            marquerModifie();
        }
    });
    connect(ui->objetRecu, &QLineEdit::textChanged, this, [this](const QString &t) {
        if (m_pageCourante >= 0 && m_pageCourante < m_pages.size()) {
            m_pages[m_pageCourante].effetObjet = t;
            marquerModifie();
        }
    });

    m_statut = new QLabel(this);
    statusBar()->addPermanentWidget(m_statut);

    creerMenusSupplementaires();
    definirRaccourcis();

    ui->editeur->setAcceptDrops(true);
    ui->editeur->installEventFilter(this);

    ui->editeur->setTextInteractionFlags(Qt::TextEditorInteraction | Qt::LinksAccessibleByMouse);
    ui->editeur->viewport()->installEventFilter(this);

    m_minuterie = new QTimer(this);
    connect(m_minuterie, &QTimer::timeout, this, &MainWindow::sauvegardeAuto);
    m_minuterie->start(60000);

    m_vueRun = new FenetreRun(this);
    ui->layoutLecture->addWidget(m_vueRun);
    connect(ui->ongletsPrincipaux, &QTabWidget::currentChanged, this, &MainWindow::changerOnglet);

    ui->nomLivre->setText(m_nomLivre);
    ui->spinPvDepart->setValue(m_departPv);
    ui->spinXpDepart->setValue(m_departXp);
    ui->objetsDepart->setText(m_departObjets);
    connect(ui->nomLivre, &QLineEdit::textChanged, this, [this](const QString &t) {
        m_nomLivre = t;
        marquerModifie();
    });
    connect(ui->spinPvDepart, &QSpinBox::valueChanged, this, [this](int v) {
        m_departPv = v;
        marquerModifie();
    });
    connect(ui->spinXpDepart, &QSpinBox::valueChanged, this, [this](int v) {
        m_departXp = v;
        marquerModifie();
    });
    connect(ui->objetsDepart, &QLineEdit::textChanged, this, [this](const QString &t) {
        m_departObjets = t;
        marquerModifie();
    });

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
    QAction *actImprimer = new QAction(QIcon(":/Icones/print.png"), "Imprimer...", this);
    actImprimer->setShortcut(QKeySequence::Print);
    connect(actImprimer, &QAction::triggered, this, &MainWindow::imprimer);
    ui->menuFichier->insertAction(ui->actionAPropos, actImprimer);

    QAction *actExport = new QAction("Exporter la page en web...", this);
    connect(actExport, &QAction::triggered, this, &MainWindow::exporterHtml);
    ui->menuFichier->insertAction(ui->actionAPropos, actExport);

    QAction *actSite = new QAction("Exporter le site complet...", this);
    connect(actSite, &QAction::triggered, this, &MainWindow::exporterSite);
    ui->menuFichier->insertAction(ui->actionAPropos, actSite);

    QAction *actPdf = new QAction("Exporter en PDF...", this);
    connect(actPdf, &QAction::triggered, this, &MainWindow::exporterPdf);
    ui->menuFichier->insertAction(ui->actionAPropos, actPdf);

    QAction *actApercu = new QAction("Aperçu avant impression...", this);
    connect(actApercu, &QAction::triggered, this, &MainWindow::apercuImpression);
    ui->menuFichier->insertAction(ui->actionAPropos, actApercu);

    ui->menuFichier->insertSeparator(ui->actionAPropos);

    ui->menuEdition->addSeparator();
    QAction *annuler  = ui->menuEdition->addAction(QIcon(":/Icones/edit_undo.png"), "Annuler");
    QAction *retablir = ui->menuEdition->addAction(QIcon(":/Icones/edit_redo.png"), "Rétablir");
    QAction *couper   = ui->menuEdition->addAction(QIcon(":/Icones/cut.png"), "Couper");
    QAction *copier   = ui->menuEdition->addAction(QIcon(":/Icones/copy.png"), "Copier");
    QAction *coller   = ui->menuEdition->addAction(QIcon(":/Icones/paste.png"), "Coller");
    QAction *toutSel  = ui->menuEdition->addAction("Tout sélectionner");
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

    ui->toolBar->addSeparator();
    ui->toolBar->addAction(annuler);
    ui->toolBar->addAction(retablir);
    ui->toolBar->addAction(couper);
    ui->toolBar->addAction(copier);
    ui->toolBar->addAction(coller);

    QMenu *menuInsertion = menuBar()->addMenu("Insertion");
    QAction *lien = menuInsertion->addAction(QIcon(":/Icones/lien.png"), "Insérer un lien...");
    lien->setShortcut(QKeySequence("Ctrl+L"));
    connect(lien, &QAction::triggered, this, &MainWindow::insererLien);
    QAction *image = menuInsertion->addAction(QIcon(":/Icones/photo.png"), "Insérer une image...");
    connect(image, &QAction::triggered, this, &MainWindow::insererImage);
    QAction *liste = menuInsertion->addAction(QIcon(":/Icones/liste.png"), "Liste à puces");
    connect(liste, &QAction::triggered, this, &MainWindow::insererListe);
    QAction *date = menuInsertion->addAction(QIcon(":/Icones/date.png"), "Insérer la date");
    connect(date, &QAction::triggered, this, [this]() {
        ui->editeur->insertPlainText(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    });

    QMenu *menuOutils = menuBar()->addMenu("Outils");
    QAction *zoomP = menuOutils->addAction(QIcon(":/Icones/zoom.png"), "Zoom avant");
    zoomP->setShortcut(QKeySequence::ZoomIn);
    connect(zoomP, &QAction::triggered, this, [this]() { ui->editeur->zoomIn(2); });
    QAction *zoomM = menuOutils->addAction(QIcon(":/Icones/zoom.png"), "Zoom arrière");
    zoomM->setShortcut(QKeySequence::ZoomOut);
    connect(zoomM, &QAction::triggered, this, [this]() { ui->editeur->zoomOut(2); });
    menuOutils->addSeparator();
    QAction *html = menuOutils->addAction("Afficher le code HTML");
    html->setCheckable(true);
    connect(html, &QAction::triggered, this, [this, html]() {
        if (html->isChecked())
            ui->editeur->setPlainText(ui->editeur->toHtml());
        else
            ui->editeur->setHtml(ui->editeur->toPlainText());
    });
    menuOutils->addSeparator();
    QAction *coherence = menuOutils->addAction("Vérifier la cohérence du livre");
    connect(coherence, &QAction::triggered, this, &MainWindow::verifierCoherenceLivre);
}

QMap<int, EffetEntree> MainWindow::construireEffets() const
{
    QMap<int, EffetEntree> effets;
    for (int i = 0; i < m_pages.size(); ++i) {
        EffetEntree e;
        e.pv = m_pages[i].effetPv;
        e.xp = m_pages[i].effetXp;
        QStringList objets = m_pages[i].effetObjet.split(',', Qt::SkipEmptyParts);
        for (int k = 0; k < objets.size(); ++k) {
            e.objets.append(objets[k].trimmed());
        }
        effets.insert(i + 1, e);
    }
    return effets;
}

Livre MainWindow::construireLivre() const
{
    Livre livre;

    for (int i = 0; i < m_pages.size(); ++i) {
        int id = i + 1;
        Page page(id, m_pages[i].titre);
        page.setTexteHtml(m_pages[i].html);

        QString titreMin = m_pages[i].titre.toLower();
        if (titreMin.contains("victoire"))
            page.setType(Page::Type::Victoire);
        else if (titreMin.contains("defaite") || titreMin.contains("défaite") || titreMin.contains("mort"))
            page.setType(Page::Type::Defaite);

        QRegularExpression reLien("<a\\b[^>]*href=\"([^\"]*)\"[^>]*>(.*?)</a>",
                                  QRegularExpression::DotMatchesEverythingOption
                                  | QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatchIterator it = reLien.globalMatch(m_pages[i].html);
        while (it.hasNext()) {
            QRegularExpressionMatch correspondance = it.next();
            QString href = correspondance.captured(1);
            QString libelle = correspondance.captured(2);
            libelle.remove(QRegularExpression("<[^>]*>"));
            libelle = libelle.trimmed();

            QString base = href;
            QString fragment;
            int diese = href.indexOf('#');
            if (diese != -1) {
                base = href.left(diese);
                fragment = href.mid(diese + 1);
            }

            int cible = -1;
            QRegularExpression reNum("(\\d+)");
            QRegularExpressionMatch mNum = reNum.match(base);
            if (mNum.hasMatch())
                cible = mNum.captured(1).toInt();

            Choix choix(libelle, cible);

            if (!fragment.isEmpty()) {
                QStringList parties = fragment.split(';', Qt::SkipEmptyParts);
                for (int k = 0; k < parties.size(); ++k) {
                    QStringList kv = parties[k].split('=');
                    if (kv.size() != 2)
                        continue;
                    QString cle = kv[0].toLower();
                    QString valeur = kv[1];
                    if (cle == "pv") {
                        choix.setDeltaPV(valeur.toInt());
                    } else if (cle == "xp") {
                        choix.setDeltaXP(valeur.toInt());
                    } else if (cle == "objet") {
                        choix.setObjetGagne(valeur);
                    } else if (cle == "requiert") {
                        Condition condition;
                        condition.setObjetRequis(valeur);
                        choix.setCondition(condition);
                    } else if (cle == "requiertpage") {
                        Condition condition;
                        condition.setPageRequise(valeur.toInt());
                        choix.setCondition(condition);
                    }
                }
            }

            page.ajouterChoix(choix);
        }

        livre.ajouterPage(page);
    }

    livre.setIdPageDepart(1);
    return livre;
}

void MainWindow::changerOnglet(int index)
{
    if (ui->ongletsPrincipaux->widget(index) == ui->ongletLecture) {
        sauvegarderPageCourante();
        QStringList objets;
        QStringList brut = m_departObjets.split(',', Qt::SkipEmptyParts);
        for (int i = 0; i < brut.size(); ++i) {
            objets.append(brut[i].trimmed());
        }
        m_vueRun->chargerLivre(construireLivre(), m_departPv, m_departXp, objets, construireEffets());
    }
}

void MainWindow::verifierCoherenceLivre()
{
    sauvegarderPageCourante();

    int n = m_pages.size();
    if (n == 0)
        return;

    QList<QList<int>> liens;
    QStringList liensCasses;
    QRegularExpression reLien("<a\\b[^>]*href=\"([^\"]*)\"[^>]*>",
                              QRegularExpression::CaseInsensitiveOption);
    QRegularExpression reNum("(\\d+)");

    for (int i = 0; i < n; ++i) {
        QList<int> cibles;
        bool aLien = false;
        QRegularExpressionMatchIterator it = reLien.globalMatch(m_pages[i].html);
        while (it.hasNext()) {
            QString href = it.next().captured(1);
            QString base = href;
            int diese = href.indexOf('#');
            if (diese != -1)
                base = href.left(diese);
            QRegularExpressionMatch mNum = reNum.match(base);
            if (mNum.hasMatch()) {
                aLien = true;
                int cible = mNum.captured(1).toInt();
                cibles.append(cible);
                if (cible < 1 || cible > n)
                    liensCasses.append("Page " + QString::number(i + 1) + " vers page "
                                       + QString::number(cible) + " (inexistante)");
            }
        }

        QString titreMin = m_pages[i].titre.toLower();
        bool estFin = titreMin.contains("victoire") || titreMin.contains("defaite")
                      || titreMin.contains("défaite") || titreMin.contains("mort");
        if (!aLien && !estFin && i + 1 < n)
            cibles.append(i + 2);

        liens.append(cibles);
    }

    QList<bool> vu;
    for (int i = 0; i < n; ++i)
        vu.append(false);

    QList<int> pile;
    pile.append(1);
    vu[0] = true;
    while (!pile.isEmpty()) {
        int p = pile.takeLast();
        QList<int> cibles = liens[p - 1];
        for (int k = 0; k < cibles.size(); ++k) {
            int c = cibles[k];
            if (c >= 1 && c <= n && !vu[c - 1]) {
                vu[c - 1] = true;
                pile.append(c);
            }
        }
    }

    QStringList inaccessibles;
    for (int i = 0; i < n; ++i) {
        if (!vu[i]) {
            QString titre = m_pages[i].titre;
            if (titre.isEmpty())
                titre = "sans titre";
            inaccessibles.append("Page " + QString::number(i + 1) + " (" + titre + ")");
        }
    }

    QString message;
    if (inaccessibles.isEmpty() && liensCasses.isEmpty()) {
        message = "Tout est cohérent : toutes les pages sont accessibles depuis la page 1 "
                  "et aucun lien ne pointe vers une page inexistante.";
    } else {
        if (!inaccessibles.isEmpty())
            message += "Pages jamais accessibles :\n- " + inaccessibles.join("\n- ") + "\n\n";
        if (!liensCasses.isEmpty())
            message += "Liens cassés :\n- " + liensCasses.join("\n- ");
    }

    QMessageBox::information(this, "Vérification de la cohérence", message);
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

void MainWindow::rafraichirListePages()
{
    ui->listePages->blockSignals(true);
    ui->listePages->clear();
    for (int i = 0; i < m_pages.size(); ++i) {
        QString t;
        if (m_pages[i].titre.isEmpty())
            t = "(sans titre)";
        else
            t = m_pages[i].titre;
        QListWidgetItem *item = new QListWidgetItem(
            "Page " + QString::number(i + 1) + " — " + t
            + "  (page" + QString::number(i + 1) + ".html)");
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

    ui->spinPv->blockSignals(true);
    ui->spinXp->blockSignals(true);
    ui->objetRecu->blockSignals(true);
    ui->spinPv->setValue(m_pages[index].effetPv);
    ui->spinXp->setValue(m_pages[index].effetXp);
    ui->objetRecu->setText(m_pages[index].effetObjet);
    ui->spinPv->blockSignals(false);
    ui->spinXp->blockSignals(false);
    ui->objetRecu->blockSignals(false);

    majStatistiques();
}

void MainWindow::sauvegarderPageCourante()
{
    if (m_pageCourante < 0 || m_pageCourante >= m_pages.size())
        return;
    m_pages[m_pageCourante].titre = ui->titre->text();
    m_pages[m_pageCourante].html = ui->editeur->toHtml();
    m_pages[m_pageCourante].effetPv = ui->spinPv->value();
    m_pages[m_pageCourante].effetXp = ui->spinXp->value();
    m_pages[m_pageCourante].effetObjet = ui->objetRecu->text();
}

void MainWindow::changerPage(QListWidgetItem *courant, QListWidgetItem *precedent)
{
    if (precedent) {
        int idx = precedent->data(Qt::UserRole).toInt();
        if (idx >= 0 && idx < m_pages.size()) {
            m_pages[idx].titre = ui->titre->text();
            m_pages[idx].html = ui->editeur->toHtml();
            m_pages[idx].effetPv = ui->spinPv->value();
            m_pages[idx].effetXp = ui->spinXp->value();
            m_pages[idx].effetObjet = ui->objetRecu->text();
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

void MainWindow::nouveau()
{
    if (!confirmerAbandonModifs())
        return;
    m_pages.clear();
    m_pageCourante = -1;
    m_pages.append({ "Page de départ", QString() });
    rafraichirListePages();
    ui->listePages->setCurrentRow(0);

    m_nomLivre = "Mon livre";
    m_departPv = 100;
    m_departXp = 0;
    m_departObjets.clear();
    ui->nomLivre->setText(m_nomLivre);
    ui->spinPvDepart->setValue(m_departPv);
    ui->spinXpDepart->setValue(m_departXp);
    ui->objetsDepart->clear();

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

QString MainWindow::nomFichierSain() const
{
    QString nom = m_nomLivre.trimmed();
    if (nom.isEmpty())
        nom = "livre";
    QString resultat;
    for (int i = 0; i < nom.size(); ++i) {
        QChar c = nom[i];
        if (c.isLetterOrNumber() || c == ' ' || c == '-' || c == '_')
            resultat += c;
        else
            resultat += '_';
    }
    return resultat;
}

bool MainWindow::enregistrerSous()
{
    const QString parent = QFileDialog::getExistingDirectory(this, "Choisir où enregistrer le livre");
    if (parent.isEmpty())
        return false;
    const QString nom = nomFichierSain();
    const QString dossier = parent + "/" + nom;
    QDir().mkpath(dossier);
    return ecrireFichier(dossier + "/" + nom + ".json");
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
        flux << ui->editeur->toHtml();
    } else if (chemin.endsWith(".txt", Qt::CaseInsensitive)) {
        flux << ui->editeur->toPlainText();
    } else {
        QJsonArray tableau;
        for (int i = 0; i < m_pages.size(); ++i) {
            QJsonObject o;
            o["titre"] = m_pages[i].titre;
            o["html"]  = m_pages[i].html;
            o["effetPv"] = m_pages[i].effetPv;
            o["effetXp"] = m_pages[i].effetXp;
            o["effetObjet"] = m_pages[i].effetObjet;
            tableau.append(o);
        }
        QJsonObject depart;
        depart["nom"] = m_nomLivre;
        depart["pv"] = m_departPv;
        depart["xp"] = m_departXp;
        depart["objets"] = m_departObjets;
        QJsonObject racine;
        racine["pages"] = tableau;
        racine["depart"] = depart;
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
        for (int i = 0; i < tableau.size(); ++i) {
            QJsonObject o = tableau[i].toObject();
            PageDoc p;
            p.titre = o["titre"].toString();
            p.html = o["html"].toString();
            p.effetPv = o["effetPv"].toInt(0);
            p.effetXp = o["effetXp"].toInt(0);
            p.effetObjet = o["effetObjet"].toString();
            m_pages.append(p);
        }
        if (m_pages.isEmpty())
            m_pages.append({ "Page de départ", QString() });

        QJsonObject depart = doc.object()["depart"].toObject();
        m_nomLivre = depart["nom"].toString();
        if (m_nomLivre.isEmpty())
            m_nomLivre = "Mon livre";
        m_departPv = depart["pv"].toInt(100);
        m_departXp = depart["xp"].toInt(0);
        m_departObjets = depart["objets"].toString();
        ui->nomLivre->blockSignals(true);
        ui->spinPvDepart->blockSignals(true);
        ui->spinXpDepart->blockSignals(true);
        ui->objetsDepart->blockSignals(true);
        ui->nomLivre->setText(m_nomLivre);
        ui->spinPvDepart->setValue(m_departPv);
        ui->spinXpDepart->setValue(m_departXp);
        ui->objetsDepart->setText(m_departObjets);
        ui->nomLivre->blockSignals(false);
        ui->spinPvDepart->blockSignals(false);
        ui->spinXpDepart->blockSignals(false);
        ui->objetsDepart->blockSignals(false);
    } else {
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

void MainWindow::apercuImpression()
{
    QPrinter imprimante;
    QPrintPreviewDialog apercu(&imprimante, this);
    connect(&apercu, &QPrintPreviewDialog::paintRequested, this, [this](QPrinter *p) {
        ui->editeur->print(p);
    });
    apercu.exec();
}

void MainWindow::exporterPdf()
{
    const QString chemin = QFileDialog::getSaveFileName(this, "Exporter en PDF",
                                                        QString(), "PDF (*.pdf)");
    if (chemin.isEmpty())
        return;

    QPrinter imprimante(QPrinter::HighResolution);
    imprimante.setOutputFormat(QPrinter::PdfFormat);
    imprimante.setOutputFileName(chemin);
    ui->editeur->document()->print(&imprimante);
    statusBar()->showMessage("PDF exporté : " + chemin, 3000);
}

void MainWindow::sauvegardeAuto()
{
    if (!m_fichierCourant.isEmpty() && isWindowModified()) {
        ecrireFichier(m_fichierCourant);
        statusBar()->showMessage("Sauvegarde automatique", 2000);
    }
}

void MainWindow::deposerFichier(const QString &chemin)
{
    QString ext = QFileInfo(chemin).suffix().toLower();

    if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp" || ext == "gif") {
        QImage image(chemin);
        if (!image.isNull())
            ui->editeur->textCursor().insertImage(image);
        return;
    }

    QFile fichier(chemin);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    const QString contenu = QString::fromUtf8(fichier.readAll());
    fichier.close();

    if (ext == "html")
        ui->editeur->insertHtml(contenu);
    else
        ui->editeur->insertPlainText(contenu);
}

bool MainWindow::eventFilter(QObject *objet, QEvent *event)
{
    if (objet == ui->editeur->viewport() && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        if (e->button() == Qt::LeftButton) {
            QString lien = ui->editeur->anchorAt(e->pos());
            if (!lien.isEmpty()) {
                QDesktopServices::openUrl(QUrl(lien));
                return true;
            }
        }
    }

    if (objet == ui->editeur) {
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent *e = static_cast<QDragEnterEvent *>(event);
            if (e->mimeData()->hasUrls()) {
                e->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            QDropEvent *e = static_cast<QDropEvent *>(event);
            if (e->mimeData()->hasUrls()) {
                QList<QUrl> urls = e->mimeData()->urls();
                if (!urls.isEmpty()) {
                    deposerFichier(urls.first().toLocalFile());
                    e->acceptProposedAction();
                    return true;
                }
            }
        }
    }
    return QMainWindow::eventFilter(objet, event);
}

QString MainWindow::corpsDeHtml(const QString &html) const
{
    QString corps = html;
    int debut = html.indexOf("<body");
    if (debut != -1) {
        debut = html.indexOf('>', debut) + 1;
        int fin = html.indexOf("</body>", debut);
        corps = html.mid(debut, fin - debut);
    }
    return corps;
}

QString MainWindow::gabaritPage(const QString &titre, const QString &corps) const
{
    const QString modele = QStringLiteral(R"HTML(<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>%1</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link href="https://fonts.googleapis.com/css2?family=Cinzel:wght@600;800&family=EB+Garamond:ital@0;1&display=swap" rel="stylesheet">
<style>
  * { box-sizing: border-box; }

  body {
    margin: 0;
    min-height: 100vh;
    padding: 48px 20px;
    background: radial-gradient(circle at 50% 0%, #3a3326, #1d1a14 70%);
    font-family: 'EB Garamond', Georgia, serif;
    color: #33271a;
  }

  .page {
    max-width: 760px;
    margin: auto;
    padding: 56px 60px;
    background: #efe2c4;
    background-image: radial-gradient(rgba(120,90,40,.07) 1px, transparent 1px);
    background-size: 13px 13px;
    border: 1px solid #b89b6e;
    border-radius: 5px;
    box-shadow: 0 0 0 7px rgba(0,0,0,.28), 0 18px 44px rgba(0,0,0,.6);
    position: relative;
    animation: apparition .6s ease;
  }

  .page::before {
    content: '';
    position: absolute;
    inset: 11px;
    border: 1px solid rgba(138,99,52,.4);
    border-radius: 3px;
    pointer-events: none;
  }

  @keyframes apparition {
    from { opacity: 0; transform: translateY(12px); }
    to   { opacity: 1; }
  }

  .ornement {
    text-align: center;
    color: #a07b46;
    letter-spacing: 6px;
    font-size: 14px;
  }

  h1.titre {
    font-family: 'Cinzel', serif;
    font-weight: 800;
    text-align: center;
    color: #5a3d23;
    font-size: 2.2rem;
    letter-spacing: 1px;
    margin: 6px 0 24px;
    text-shadow: 1px 1px 0 rgba(255,255,255,.45);
  }

  .contenu p {
    line-height: 1.85;
    font-size: 1.15rem;
    text-align: justify;
  }

  .contenu > p:first-of-type {
    overflow: hidden;
  }

  .contenu > p:first-of-type::first-letter {
    float: left;
    font-family: 'Cinzel', serif;
    font-size: 3.6rem;
    line-height: .8;
    color: #8b3a1d;
    padding: 6px 10px 0 0;
  }

  img {
    max-width: 100%;
    display: block;
    margin: 20px auto;
    border-radius: 4px;
    box-shadow: 0 6px 18px rgba(0,0,0,.35);
  }

  a {
    display: inline-block;
    margin: 8px;
    padding: 13px 26px;
    background: linear-gradient(#9c6a3b, #7a4f29);
    color: #fff5e6;
    text-decoration: none;
    border: 1px solid #5a3a1c;
    border-radius: 8px;
    font-family: 'Cinzel', serif;
    font-weight: 600;
    letter-spacing: .5px;
    box-shadow: 0 4px 0 #4a3017, 0 6px 12px rgba(0,0,0,.4);
    transition: all .15s ease;
  }

  a:hover  { transform: translateY(-2px); box-shadow: 0 6px 0 #4a3017, 0 10px 18px rgba(0,0,0,.45); }
  a:active { transform: translateY(2px);  box-shadow: 0 1px 0 #4a3017; }

  .hud {
    background: #2a2018;
    color: #f4ead3;
    font-family: 'Cinzel', serif;
    text-align: center;
    padding: 10px 14px;
    border-radius: 6px;
    margin-bottom: 22px;
    font-size: 0.95rem;
  }

  .hud a {
    display: inline;
    margin: 0 0 0 12px;
    padding: 0;
    background: none;
    border: none;
    box-shadow: none;
    color: #d9a441;
    font-size: 0.85rem;
  }
</style>
</head>
<body>
  <div class="page">
    <div class="ornement">&#10022; &#10022; &#10022;</div>
    <h1 class="titre">%1</h1>
    <div class="contenu">
%2
    </div>
  </div>
</body>
</html>
)HTML");

    return modele.arg(titre, corps);
}

void MainWindow::exporterHtml()
{
    sauvegarderPageCourante();

    const QString chemin = QFileDialog::getSaveFileName(this, "Exporter en page web",
                                                        QString(), "Page web (*.html)");
    if (chemin.isEmpty())
        return;

    QString titre;
    if (ui->titre->text().isEmpty())
        titre = "Mon livre";
    else
        titre = ui->titre->text();

    const QString page = gabaritPage(titre, corpsDeHtml(ui->editeur->toHtml()));

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

void MainWindow::exporterSite()
{
    sauvegarderPageCourante();

    const QString parent = QFileDialog::getExistingDirectory(this, "Choisir un dossier pour le site");
    if (parent.isEmpty())
        return;
    const QString dossier = parent + "/" + nomFichierSain();
    QDir().mkpath(dossier);

    const QString scriptJeu = QStringLiteral(R"JS(
<script>
function chargerEtat() {
  var brut = localStorage.getItem("ldvelh");
  if (brut === null) {
    return { pv: 100, xp: 0, objets: [], pages: [], historique: [] };
  }
  return JSON.parse(brut);
}
function sauverEtat(etat) {
  localStorage.setItem("ldvelh", JSON.stringify(etat));
}
function definirValeursDepart(etat, fragment) {
  var parties = fragment.split(";");
  for (var i = 0; i < parties.length; i++) {
    var kv = parties[i].split("=");
    if (kv.length !== 2) {
      continue;
    }
    var cle = kv[0].trim();
    var valeur = kv[1].trim();
    if (cle === "pv") {
      etat.pv = parseInt(valeur);
    } else if (cle === "xp") {
      etat.xp = parseInt(valeur);
    } else if (cle === "objet") {
      if (etat.objets.indexOf(valeur) === -1) {
        etat.objets.push(valeur);
      }
    }
  }
}
function afficherHud(etat) {
  document.getElementById("hud-pv").textContent = etat.pv;
  document.getElementById("hud-xp").textContent = etat.xp;
  if (etat.objets.length === 0) {
    document.getElementById("hud-objets").textContent = "aucun";
  } else {
    document.getElementById("hud-objets").textContent = etat.objets.join(", ");
  }
}
function lireFragment(href) {
  var diese = href.indexOf("#");
  if (diese === -1) {
    return "";
  }
  return href.substring(diese + 1);
}
function baseHref(href) {
  var diese = href.indexOf("#");
  if (diese === -1) {
    return href;
  }
  return href.substring(0, diese);
}
function appliquerEffets(etat, fragment) {
  var parties = fragment.split(";");
  for (var i = 0; i < parties.length; i++) {
    var kv = parties[i].split("=");
    if (kv.length !== 2) {
      continue;
    }
    var cle = kv[0].trim();
    var valeur = kv[1].trim();
    if (cle === "pv") {
      etat.pv += parseInt(valeur);
    } else if (cle === "xp") {
      etat.xp += parseInt(valeur);
    } else if (cle === "objet") {
      if (etat.objets.indexOf(valeur) === -1) {
        etat.objets.push(valeur);
      }
    }
  }
}
function conditionRemplie(etat, fragment) {
  var parties = fragment.split(";");
  for (var i = 0; i < parties.length; i++) {
    var kv = parties[i].split("=");
    if (kv.length !== 2) {
      continue;
    }
    var cle = kv[0].trim();
    var valeur = kv[1].trim();
    if (cle === "requiert") {
      if (etat.objets.indexOf(valeur) === -1) {
        return false;
      }
    }
    if (cle === "requiertpage") {
      if (etat.pages.indexOf(parseInt(valeur)) === -1) {
        return false;
      }
    }
  }
  return true;
}
window.addEventListener("DOMContentLoaded", function() {
  var pageActuelle = %1;
  var initJeu = "%2";
  var effetPage = "%3";
  var brut = localStorage.getItem("ldvelh");
  var etat;
  if (brut === null) {
    etat = { pv: 100, xp: 0, objets: [], pages: [], historique: [] };
    if (initJeu !== "") {
      definirValeursDepart(etat, initJeu);
    }
  } else {
    etat = JSON.parse(brut);
  }
  if (etat.pages.indexOf(pageActuelle) === -1) {
    if (effetPage !== "") {
      appliquerEffets(etat, effetPage);
    }
    etat.pages.push(pageActuelle);
  }
  sauverEtat(etat);
  if (etat.pv <= 0) {
    alert("Vous avez succombé. Retour au début de l'aventure.");
    localStorage.removeItem("ldvelh");
    window.location.href = "index.html";
    return;
  }
  afficherHud(etat);
  var liens = document.querySelectorAll(".contenu a");
  for (var i = 0; i < liens.length; i++) {
    var lien = liens[i];
    if (lien.classList.contains("lien-retour")) {
      if (etat.historique.length === 0) {
        lien.style.display = "none";
      } else {
        lien.addEventListener("click", function(e) {
          e.preventDefault();
          var et = chargerEtat();
          if (et.historique.length === 0) {
            window.location.href = "index.html";
            return;
          }
          var snap = et.historique.pop();
          et.pv = snap.pv;
          et.xp = snap.xp;
          et.objets = snap.objets;
          et.pages = snap.pages;
          sauverEtat(et);
          window.location.href = "page" + snap.page + ".html";
        });
      }
      continue;
    }
    var href = lien.getAttribute("href");
    if (baseHref(href) === "index.html") {
      continue;
    }
    var fragment = lireFragment(href);
    if (fragment !== "" && conditionRemplie(etat, fragment) === false) {
      lien.style.display = "none";
    } else {
      lien.addEventListener("click", function(e) {
        e.preventDefault();
        var h = this.getAttribute("href");
        var et = chargerEtat();
        et.historique.push({ page: pageActuelle, pv: et.pv, xp: et.xp, objets: et.objets.slice(), pages: et.pages.slice() });
        var frag = lireFragment(h);
        if (frag !== "") {
          appliquerEffets(et, frag);
        }
        sauverEtat(et);
        if (et.pv <= 0) {
          alert("Vous avez succombé. Retour au début de l'aventure.");
          localStorage.removeItem("ldvelh");
          window.location.href = "index.html";
        } else {
          window.location.href = baseHref(h);
        }
      });
    }
  }
});
</script>
)JS");

    for (int i = 0; i < m_pages.size(); ++i) {
        QString titre = m_pages[i].titre;
        if (titre.isEmpty())
            titre = "Page " + QString::number(i + 1);

        QString initStr;
        if (i == 0) {
            initStr = "pv=" + QString::number(m_departPv) + ";xp=" + QString::number(m_departXp);
            QStringList objets = m_departObjets.split(',', Qt::SkipEmptyParts);
            for (int k = 0; k < objets.size(); ++k) {
                initStr += ";objet=" + objets[k].trimmed();
            }
        }

        QString effetStr;
        if (i != 0) {
            effetStr = "pv=" + QString::number(m_pages[i].effetPv)
                       + ";xp=" + QString::number(m_pages[i].effetXp);
            QStringList objetsEffet = m_pages[i].effetObjet.split(',', Qt::SkipEmptyParts);
            for (int k = 0; k < objetsEffet.size(); ++k) {
                effetStr += ";objet=" + objetsEffet[k].trimmed();
            }
        }

        QString corps = "<div class=\"hud\">PV : <span id=\"hud-pv\"></span> · XP : <span id=\"hud-xp\"></span>"
                        " · Objets : <span id=\"hud-objets\"></span>"
                        "<a href=\"index.html\">Recommencer</a></div>\n";
        corps += corpsDeHtml(m_pages[i].html);

        bool aDesLiens = m_pages[i].html.contains("<a ", Qt::CaseInsensitive);
        QString titreMin = m_pages[i].titre.toLower();
        bool estFin = titreMin.contains("victoire") || titreMin.contains("defaite")
                      || titreMin.contains("défaite") || titreMin.contains("mort");
        if (!aDesLiens && !estFin && i + 1 < m_pages.size()) {
            corps += "\n<div style=\"text-align:center; margin-top:24px;\">"
                     "<a href=\"page" + QString::number(i + 2) + ".html\">Continuer →</a></div>";
        }

        corps += "\n<div style=\"text-align:center; margin-top:18px;\">"
                 "<a class=\"lien-retour\" href=\"index.html\">← Retour</a></div>";
        corps += "\n<div style=\"text-align:center; margin-top:12px;\">"
                 "<a href=\"index.html\">↩ Retour au sommaire</a></div>";
        corps += scriptJeu.arg(i + 1).arg(initStr).arg(effetStr);

        const QString page = gabaritPage(titre, corps);

        QFile f(dossier + "/page" + QString::number(i + 1) + ".html");
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream s(&f);
            s << page;
            f.close();
        }
    }

    QString marqueurs;
    for (int i = 0; i < m_pages.size(); ++i) {
        QString titre = m_pages[i].titre;
        if (titre.isEmpty())
            titre = "Page " + QString::number(i + 1);
        titre.replace("\"", "\\\"");

        double lat = i * 6.5;
        double lng = i * 12.0;

        marqueurs += "    L.marker([" + QString::number(lat) + ", " + QString::number(lng) + "]).addTo(map)\n";
        marqueurs += "      .bindPopup(\"<div class='bulle'><b>Page " + QString::number(i + 1) + "</b><br>"
                     + titre + "<br><br>"
                     + "<a href='page" + QString::number(i + 1) + ".html'>Entrer dans ce chapitre</a></div>\");\n";
    }

    QString sommaire;
    for (int i = 0; i < m_pages.size(); ++i) {
        QString titre = m_pages[i].titre;
        if (titre.isEmpty())
            titre = "Page " + QString::number(i + 1);
        sommaire += "<li><a href=\"page" + QString::number(i + 1) + ".html\">" + titre + "</a></li>\n";
    }

    const QString modeleIndex = QStringLiteral(R"HTML(<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Carte de l'aventure</title>
<link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
<script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link href="https://fonts.googleapis.com/css2?family=Cinzel:wght@600;800&family=EB+Garamond:ital@0;1&display=swap" rel="stylesheet">
<style>
  * { box-sizing: border-box; }
  body {
    margin: 0;
    min-height: 100vh;
    padding: 48px 20px;
    background: radial-gradient(circle at 50% 0%, #3a3326, #1d1a14 70%);
    font-family: 'EB Garamond', Georgia, serif;
    color: #fff5e6;
    text-align: center;
  }
  h1 { font-family: 'Cinzel', serif; font-weight: 800; color: #f4ead3; margin-bottom: 4px; }
  .description { color: #a07b46; font-style: italic; font-size: 1.2rem; margin-bottom: 24px; }
  #map {
    max-width: 900px;
    height: 540px;
    margin: 0 auto;
    border-radius: 8px;
    border: 2px solid #b89b6e;
    box-shadow: 0 0 0 6px rgba(0,0,0,.25), 0 15px 40px rgba(0,0,0,.5);
  }
  .bulle { font-family: 'EB Garamond', serif; color: #2a2018; font-size: 1.1rem; }
  .bulle a { font-family: 'Cinzel', serif; font-weight: bold; color: #9c6a3b; text-decoration: none; }
  .bulle a:hover { text-decoration: underline; }
  .demarrer {
    display: inline-block;
    margin: 26px 0 10px;
    padding: 13px 30px;
    background: linear-gradient(#9c6a3b, #7a4f29);
    color: #fff5e6;
    text-decoration: none;
    border: 1px solid #5a3a1c;
    border-radius: 8px;
    font-family: 'Cinzel', serif;
    font-weight: 600;
    box-shadow: 0 4px 0 #4a3017, 0 6px 12px rgba(0,0,0,.4);
  }
  h2 { font-family: 'Cinzel', serif; color: #f4ead3; font-size: 1.3rem; margin-top: 30px; }
  ol.sommaire { max-width: 440px; margin: 8px auto 0; text-align: left; line-height: 2; padding-left: 20px; }
  ol.sommaire a { color: #f4ead3; }
</style>
</head>
<body>
  <h1>$TITRELIVRE$</h1>
  <p class="description">Carte des étapes du récit</p>
  <div id="map"></div>

  <div><a class="demarrer" href="page1.html">Commencer l'aventure</a></div>

  <h2>Sommaire</h2>
  <ol class="sommaire">
%2
  </ol>

  <script>
    localStorage.removeItem("ldvelh");
    var map = L.map('map').setView([10, 20], 3);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      maxZoom: 18,
      attribution: '© OpenStreetMap'
    }).addTo(map);
%1
  </script>
</body>
</html>
)HTML");

    QString index = modeleIndex.arg(marqueurs, sommaire);
    QString nomAffiche = m_nomLivre.trimmed();
    if (nomAffiche.isEmpty())
        nomAffiche = "Mon livre";
    index.replace("$TITRELIVRE$", nomAffiche.toHtmlEscaped());
    QFile fi(dossier + "/index.html");
    if (fi.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream s(&fi);
        s << index;
        fi.close();
    }

    statusBar()->showMessage("Site exporté dans : " + dossier, 4000);
    QMessageBox::information(this, "Export du site",
                            "Site exporté !\nOuvre index.html dans ton navigateur.\n\n"
                            "Astuce : pour que les liens marchent, mets comme cible "
                            "« page2.html », « page3.html »… dans Insérer un lien.");
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
    if (ui->actionGras->isChecked())
        ui->editeur->setFontWeight(QFont::Bold);
    else
        ui->editeur->setFontWeight(QFont::Normal);
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
    QString nom;
    if (m_fichierCourant.isEmpty())
        nom = "Nouveau livre";
    else
        nom = QFileInfo(m_fichierCourant).fileName();
    setWindowTitle(nom + "[*] - Éditeur LDVELH");
}
