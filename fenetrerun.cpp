#include "fenetrerun.h"
#include "ui_fenetrerun.h"

#include <QLabel>
#include <QLayoutItem>
#include <QPushButton>
#include <QRegularExpression>
#include <QStringList>

FenetreRun::FenetreRun(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FenetreRun)
    , m_pageCourante(-1)
{
    ui->setupUi(this);
    ui->zoneTexte->setReadOnly(true);
    ui->zoneTexte->setOpenLinks(false);

    connect(ui->btnRecommencer, &QPushButton::clicked, this, &FenetreRun::recommencer);
}

FenetreRun::~FenetreRun()
{
    delete ui;
}

void FenetreRun::chargerLivre(const Livre &livre)
{
    m_livre = livre;
    m_etat = EtatJoueur();
    m_etat.pv = 100;
    m_etat.xp = 0;
    afficherPage(m_livre.getIdPageDepart());
}

void FenetreRun::afficherPage(int id)
{
    m_pageCourante = id;

    if (!m_livre.contientPage(id)) {
        afficherFin("Page introuvable.");
        return;
    }

    Page &page = m_livre.getPage(id);
    m_etat.pagesVisitees.insert(id);

    ui->etiquetteTitre->setText(page.titre());

    QString texte = page.texteHtml();
    QRegularExpression reLien("<a\\b[^>]*>.*?</a>",
                              QRegularExpression::DotMatchesEverythingOption
                              | QRegularExpression::CaseInsensitiveOption);
    texte.remove(reLien);
    ui->zoneTexte->setHtml(texte);

    majEtat();
    viderChoix();

    if (page.estFin()) {
        if (page.type() == Page::Type::Victoire)
            afficherFin("Victoire ! Vous avez triomphé.");
        else
            afficherFin("Défaite… Votre aventure s'achève ici.");
        return;
    }

    if (m_etat.estMort()) {
        afficherFin("Vous avez succombé. Fin de l'aventure.");
        return;
    }

    int nbVisibles = 0;
    QVector<Choix> &listeChoix = page.choix();
    for (int i = 0; i < listeChoix.size(); ++i) {
        if (listeChoix[i].estVisible(m_etat)) {
            Choix choix = listeChoix[i];
            QPushButton *bouton = new QPushButton(choix.texte(), ui->conteneurChoix);
            connect(bouton, &QPushButton::clicked, this, [this, choix]() {
                allerVersPage(choix);
            });
            ui->layoutChoix->addWidget(bouton);
            nbVisibles++;
        }
    }

    if (nbVisibles == 0)
        afficherFin("Fin de l'aventure.");
}

void FenetreRun::allerVersPage(const Choix &choix)
{
    choix.appliquerEffets(m_etat);

    if (m_etat.estMort()) {
        majEtat();
        afficherFin("Vous avez succombé. Fin de l'aventure.");
        return;
    }

    afficherPage(choix.pageCible());
}

void FenetreRun::majEtat()
{
    QString objets;
    if (m_etat.objets.isEmpty())
        objets = "aucun";
    else
        objets = QStringList(m_etat.objets.values()).join(", ");

    ui->etiquetteEtat->setText(
        QString("PV : %1   |   XP : %2   |   Objets : %3")
            .arg(m_etat.pv).arg(m_etat.xp).arg(objets));
}

void FenetreRun::viderChoix()
{
    QLayoutItem *item;
    while ((item = ui->layoutChoix->takeAt(0)) != nullptr) {
        if (item->widget() != nullptr)
            item->widget()->deleteLater();
        delete item;
    }
}

void FenetreRun::afficherFin(const QString &message)
{
    viderChoix();
    QLabel *etiquette = new QLabel(message, ui->conteneurChoix);
    etiquette->setAlignment(Qt::AlignCenter);
    etiquette->setStyleSheet(
        "font-family:'Cinzel'; font-size:18px; font-weight:bold; color:#5a3d23; padding:14px;");
    ui->layoutChoix->addWidget(etiquette);
}

void FenetreRun::recommencer()
{
    m_etat = EtatJoueur();
    m_etat.pv = 100;
    m_etat.xp = 0;
    afficherPage(m_livre.getIdPageDepart());
}
