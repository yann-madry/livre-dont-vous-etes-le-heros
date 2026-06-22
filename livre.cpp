#include "livre.h"

#include <QFile>
#include <QTextStream>
#include <QString>

Livre::Livre() {
    this->idPageDep = 1;
    this->pageErreur.setTitre("Page introuvable");
    this->pageErreur.setId(-1);
}

int Livre::getIdPageDepart() const {
    return this->idPageDep;
}

void Livre::setIdPageDepart(int id) {
    this->idPageDep = id;
}

void Livre::ajouterPage(const Page &nouvellePage) {
    this->livre.insert(nouvellePage.id(), nouvellePage);
}

bool Livre::contientPage(int id) const {
    if (this->livre.contains(id))
        return true;
    else
        return false;
}

Page& Livre::getPage(int id) {
    if (Livre::contientPage(id))
        return this->livre[id];
    return this->pageErreur;
}

void Livre::supprimerPage(int id) {
    if (Livre::contientPage(id))
        this->livre.remove(id);
}

QMap<int,Page>& Livre::getAllPages() {
    return this->livre;
}

void Livre::exporterEnSiteWeb(const QString &cheminDossier)
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
    color: #2a2018;
  }

  .page {
    max-width: 760px;
    margin: auto;
    background: #f4ead3;
    background-image: radial-gradient(rgba(120,90,40,.06) 1px, transparent 1px);
    background-size: 14px 14px;
    padding: 56px 60px;
    border-radius: 6px;
    border: 1px solid #b89b6e;
    box-shadow: 0 0 0 8px rgba(0,0,0,.25), 0 20px 50px rgba(0,0,0,.6);
    position: relative;
  }

  .page::before {
    content: '';
    position: absolute;
    inset: 14px;
    border: 1px solid rgba(138,99,52,.45);
    border-radius: 3px;
    pointer-events: none;
  }

  h1.titre {
    font-family: 'Cinzel', serif;
    font-weight: 800;
    text-align: center;
    color: #5a3d23;
    font-size: 2.2rem;
    margin: 0 0 6px;
    text-shadow: 1px 1px 0 rgba(255,255,255,.4);
  }

  .ornement {
    text-align: center;
    color: #a07b46;
    letter-spacing: 6px;
    margin-bottom: 28px;
  }

  p {
    line-height: 1.85;
    font-size: 1.15rem;
    text-align: justify;
  }

  img {
    max-width: 100%;
    border-radius: 4px;
    display: block;
    margin: 20px auto;
    box-shadow: 0 6px 18px rgba(0,0,0,.35);
  }

  .zone-choix {
    text-align: center;
    margin-top: 34px;
  }

  .btn-choix {
    display: inline-block;
    margin: 8px;
    padding: 13px 26px;
    background: linear-gradient(#9c6a3b, #7a4f29);
    color: #fff5e6;
    text-decoration: none;
    border-radius: 8px;
    font-family: 'Cinzel', serif;
    font-weight: 600;
    letter-spacing: .5px;
    border: 1px solid #5a3a1c;
    box-shadow: 0 4px 0 #4a3017, 0 6px 12px rgba(0,0,0,.4);
    transition: all .15s ease;
  }

  .btn-choix:hover { transform: translateY(-2px); box-shadow: 0 6px 0 #4a3017, 0 10px 18px rgba(0,0,0,.45); }
  .btn-choix:active { transform: translateY(2px); box-shadow: 0 1px 0 #4a3017; }

  .navigation-carte {
    text-align: center;
    margin-top: 40px;
    padding-top: 20px;
    border-top: 1px dashed #b89b6e;
  }

  .btn-carte { color: #7a4f29; text-decoration: none; font-family: 'Cinzel', serif; font-size: 0.9rem; font-weight: bold; }
  .btn-carte:hover { text-decoration: underline; }
</style>
</head>
<body>
  <div class="page">
    <h1 class="titre">%1</h1>
    <div class="ornement">&#10070; &#10070; &#10070;</div>
    %2
    <div class="zone-choix">
%3
    </div>
    <div class="navigation-carte">
      <a class="btn-carte" href="index.html">Retourner a la carte du monde</a>
    </div>
  </div>
</body>
</html>
)HTML");

    for (auto it = this->livre.begin(); it != this->livre.end(); ++it) {
        Page p = it.value();

        QString corps = p.texteHtml();
        int debut = corps.indexOf("<body");
        if (debut != -1) {
            debut = corps.indexOf('>', debut) + 1;
            int fin = corps.indexOf("</body>", debut);
            corps = corps.mid(debut, fin - debut);
        }

        QString choix;
        QVector<Choix> liste = p.choix();
        for (int i = 0; i < liste.size(); ++i) {
            choix += "      <a class=\"btn-choix\" href=\"page_"
                     + QString::number(liste[i].pageCible()) + ".html\">"
                     + liste[i].texte() + "</a>\n";
        }

        QString page = modele.arg(p.titre()).arg(corps).arg(choix);

        QFile fichier(cheminDossier + "/page_" + QString::number(p.id()) + ".html");
        if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream flux(&fichier);
            flux.setEncoding(QStringConverter::Utf8);
            flux << page;
            fichier.close();
        }
    }
}

void Livre::genererIndexCartographique(const QString &cheminDossier)
{
    QString marqueurs;
    int index = 0;
    for (auto it = this->livre.begin(); it != this->livre.end(); ++it) {
        Page p = it.value();
        double lat = index * 6.5;
        double lng = index * 12.0;
        QString titre = p.titre();
        titre.replace("\"", "\\\"");

        marqueurs += "    L.marker([" + QString::number(lat) + ", " + QString::number(lng) + "]).addTo(map)\n";
        marqueurs += "      .bindPopup(\"<div class='bulle'><b>Etape " + QString::number(p.id()) + "</b><br>"
                     + titre + "<br><br>\" +\n";
        marqueurs += "      \"<a href='page_" + QString::number(p.id()) + ".html'>Entrer dans ce chapitre</a></div>\");\n\n";
        index++;
    }

    const QString modele = QStringLiteral(R"HTML(<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Carte de l'Aventure</title>
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

  h1 { font-family: 'Cinzel', serif; font-weight: 800; color: #f4ead3; margin-bottom: 5px; }
  .description { color: #a07b46; font-style: italic; font-size: 1.2rem; margin-bottom: 25px; }

  #map {
    max-width: 900px;
    height: 550px;
    margin: 0 auto;
    border-radius: 8px;
    border: 2px solid #b89b6e;
    box-shadow: 0 0 0 6px rgba(0,0,0,.25), 0 15px 40px rgba(0,0,0,.5);
  }

  .bulle { font-family: 'EB Garamond', serif; color: #2a2018; font-size: 1.1rem; }
  .bulle a { font-family: 'Cinzel', serif; font-weight: bold; color: #9c6a3b; text-decoration: none; }
  .bulle a:hover { text-decoration: underline; }
</style>
</head>
<body>
  <h1>Chroniques de l'Aventure</h1>
  <p class="description">Carte de localisation des etapes du recit</p>
  <div id="map"></div>

  <script>
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

    QFile fichier(cheminDossier + "/index.html");
    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream flux(&fichier);
        flux.setEncoding(QStringConverter::Utf8);
        flux << modele.arg(marqueurs);
        fichier.close();
    }
}
