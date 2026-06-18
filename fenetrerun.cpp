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
    , m_departPv(100)
    , m_departXp(0)
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

void FenetreRun::chargerLivre(const Livre &livre, int departPv, int departXp,
                              const QStringList &departObjets, const QMap<int, EffetEntree> &effets)
{
    m_livre = livre;
    m_departPv = departPv;
    m_departXp = departXp;
    m_departObjets = departObjets;
    m_effets = effets;
    demarrer();
}

void FenetreRun::demarrer()
{
    m_historiquePage.clear();
    m_historiqueEtat.clear();
    m_etat = EtatJoueur();
    m_etat.pv = m_departPv;
    m_etat.xp = m_departXp;
    for (int i = 0; i < m_departObjets.size(); ++i) {
        m_etat.objets.insert(m_departObjets[i]);
    }
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

    if (!m_etat.pagesVisitees.contains(id) && m_effets.contains(id) && id != m_livre.getIdPageDepart()) {
        EffetEntree e = m_effets.value(id);
        m_etat.pv += e.pv;
        m_etat.xp += e.xp;
        for (int k = 0; k < e.objets.size(); ++k) {
            m_etat.objets.insert(e.objets[k]);
        }
    }
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

    if (m_etat.estMort()) {
        afficherFin("Vous avez succombé. Fin de l'aventure.");
        ajouterBoutonRetour();
        return;
    }

    if (page.estFin()) {
        if (page.type() == Page::Type::Victoire) {
            afficherFin("Victoire ! Vous avez triomphé.");
        } else {
            afficherFin("Défaite… Votre aventure s'achève ici.");
        }
        ajouterBoutonRetour();
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

    if (nbVisibles == 0) {
        if (m_livre.contientPage(id + 1)) {
            QPushButton *bouton = new QPushButton("Continuer →", ui->conteneurChoix);
            connect(bouton, &QPushButton::clicked, this, [this, id]() {
                m_historiquePage.append(id);
                m_historiqueEtat.append(m_etat);
                afficherPage(id + 1);
            });
            ui->layoutChoix->addWidget(bouton);
        } else {
            afficherFin("Fin de l'aventure.");
        }
    }

    ajouterBoutonRetour();
}

void FenetreRun::allerVersPage(const Choix &choix)
{
    m_historiquePage.append(m_pageCourante);
    m_historiqueEtat.append(m_etat);

    choix.appliquerEffets(m_etat);

    if (m_etat.estMort()) {
        majEtat();
        afficherFin("Vous avez succombé. Fin de l'aventure.");
        ajouterBoutonRetour();
        return;
    }

    afficherPage(choix.pageCible());
}

void FenetreRun::retour()
{
    if (m_historiquePage.isEmpty()) {
        return;
    }
    m_etat = m_historiqueEtat.takeLast();
    int page = m_historiquePage.takeLast();
    afficherPage(page);
}

void FenetreRun::ajouterBoutonRetour()
{
    if (m_historiquePage.isEmpty()) {
        return;
    }
    QPushButton *bouton = new QPushButton("← Retour", ui->conteneurChoix);
    connect(bouton, &QPushButton::clicked, this, &FenetreRun::retour);
    ui->layoutChoix->addWidget(bouton);
}

void FenetreRun::majEtat()
{
    QString objets;
    if (m_etat.objets.isEmpty()) {
        objets = "aucun";
    } else {
        objets = QStringList(m_etat.objets.values()).join(", ");
    }

    ui->etiquetteEtat->setText(
        QString("PV : %1   |   XP : %2   |   Objets : %3")
            .arg(m_etat.pv).arg(m_etat.xp).arg(objets));
}

void FenetreRun::viderChoix()
{
    QLayoutItem *item;
    while ((item = ui->layoutChoix->takeAt(0)) != nullptr) {
        if (item->widget() != nullptr) {
            item->widget()->deleteLater();
        }
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
    demarrer();
}
