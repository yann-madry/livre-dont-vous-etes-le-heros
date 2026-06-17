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
    for (auto it = this->livre.begin(); it != this->livre.end(); ++it) {
        Page p = it.value();

        QString nomFichier = cheminDossier + "/page_" + QString::number(p.id()) + ".html";
        QFile fichier(nomFichier);

        if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream flux(&fichier);
            flux.setEncoding(QStringConverter::Utf8);

            QString htmlEditeur = p.texteHtml();
            QString corps = htmlEditeur;
            int debut = htmlEditeur.indexOf("<body");
            if (debut != -1) {
                debut = htmlEditeur.indexOf('>', debut) + 1;
                int fin = htmlEditeur.indexOf("</body>", debut);
                corps = htmlEditeur.mid(debut, fin - debut);
            }

            flux << "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n<meta charset=\"UTF-8\">\n";
            flux << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
            flux << "<title>" << p.titre() << "</title>\n";
            flux << "<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n";
            flux << "<link href=\"https://fonts.googleapis.com/css2?family=Cinzel:wght@600;800&family=EB+Garamond:ital@0;1&display=swap\" rel=\"stylesheet\">\n";

            flux << "<style>\n"
                 << "  * { box-sizing:border-box; }\n"
                 << "  body { margin:0; min-height:100vh; padding:48px 20px;\n"
                 << "         background:radial-gradient(circle at 50% 0%, #3a3326, #1d1a14 70%);\n"
                 << "         font-family:'EB Garamond', Georgia, serif; color:#2a2018; }\n"
                 << "  .page { max-width:760px; margin:auto; background:#f4ead3;\n"
                 << "          background-image:radial-gradient(rgba(120,90,40,.06) 1px, transparent 1px);\n"
                 << "          background-size:14px 14px;\n"
                 << "          padding:56px 60px; border-radius:6px;\n"
                 << "          border:1px solid #b89b6e;\n"
                 << "          box-shadow:0 0 0 8px rgba(0,0,0,.25), 0 20px 50px rgba(0,0,0,.6);\n"
                 << "          position:relative; }\n"
                 << "  .page::before { content:''; position:absolute; inset:14px;\n"
                 << "          border:1px solid rgba(138,99,52,.45); border-radius:3px; pointer-events:none; }\n"
                 << "  h1.titre { font-family:'Cinzel', serif; font-weight:800; text-align:center;\n"
                 << "             color:#5a3d23; font-size:2.2rem; margin:0 0 6px;\n"
                 << "             text-shadow:1px 1px 0 rgba(255,255,255,.4); }\n"
                 << "  .ornement { text-align:center; color:#a07b46; letter-spacing:6px; margin-bottom:28px; }\n"
                 << "  p { line-height:1.85; font-size:1.15rem; text-align:justify; }\n"
                 << "  img { max-width:100%; border-radius:4px; display:block; margin:20px auto;\n"
                 << "        box-shadow:0 6px 18px rgba(0,0,0,.35); }\n"
                 << "  .zone-choix { text-align:center; margin-top:34px; }\n"
                 << "  .btn-choix { display:inline-block; margin:8px; padding:13px 26px;\n"
                 << "        background:linear-gradient(#9c6a3b, #7a4f29); color:#fff5e6;\n"
                 << "        text-decoration:none; border-radius:8px; font-family:'Cinzel',serif;\n"
                 << "        font-weight:600; letter-spacing:.5px; border:1px solid #5a3a1c;\n"
                 << "        box-shadow:0 4px 0 #4a3017, 0 6px 12px rgba(0,0,0,.4);\n"
                 << "        transition:all .15s ease; }\n"
                 << "  .btn-choix:hover { transform:translateY(-2px); box-shadow:0 6px 0 #4a3017, 0 10px 18px rgba(0,0,0,.45); }\n"
                 << "  .btn-choix:active { transform:translateY(2px); box-shadow:0 1px 0 #4a3017; }\n"
                 << "  .navigation-carte { text-align:center; margin-top:40px; padding-top:20px; border-top:1px dashed #b89b6e; }\n"
                 << "  .btn-carte { color:#7a4f29; text-decoration:none; font-family:'Cinzel', serif; font-size:0.9rem; font-weight:bold; }\n"
                 << "  .btn-carte:hover { text-decoration:underline; }\n"
                 << "</style>\n</head>\n<body>\n";

            flux << "  <div class=\"page\">\n";
            flux << "    <h1 class=\"titre\">" << p.titre() << "</h1>\n";
            flux << "    <div class=\"ornement\">&#10070; &#10070; &#10070;</div>\n";
            flux << "    " << corps << "\n";

            flux << "    <div class=\"zone-choix\">\n";
            for (const Choix &c : p.choix()) {
                flux << "      <a class=\"btn-choix\" href=\"page_" << QString::number(c.pageCible()) << ".html\">"
                     << c.texte() << "</a>\n";
            }
            flux << "    </div>\n";

            flux << "    <div class=\"navigation-carte\">\n";
            flux << "      <a class=\"btn-carte\" href=\"index.html\">Retourner a la carte du monde</a>\n";
            flux << "    </div>\n";

            flux << "  </div>\n</body>\n</html>\n";
            fichier.close();
        }
    }
}

void Livre::genererIndexCartographique(const QString &cheminDossier)
{
    QString nomFichier = cheminDossier + "/index.html";
    QFile fichier(nomFichier);

    if (fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream flux(&fichier);
        flux.setEncoding(QStringConverter::Utf8);

        flux << "<!DOCTYPE html>\n<html lang=\"fr\">\n<head>\n<meta charset=\"UTF-8\">\n";
        flux << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
        flux << "<title>Carte de l'Aventure</title>\n";

        flux << "<link rel=\"stylesheet\" href=\"https://unpkg.com/leaflet@1.9.4/dist/leaflet.css\" />\n";
        flux << "<script src=\"https://unpkg.com/leaflet@1.9.4/dist/leaflet.js\"></script>\n";

        flux << "<link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n";
        flux << "<link href=\"https://fonts.googleapis.com/css2?family=Cinzel:wght@600;800&family=EB+Garamond:ital@0;1&display=swap\" rel=\"stylesheet\">\n";

        flux << "<style>\n"
             << "  * { box-sizing:border-box; }\n"
             << "  body { margin:0; min-height:100vh; padding:48px 20px;\n"
             << "         background:radial-gradient(circle at 50% 0%, #3a3326, #1d1a14 70%);\n"
             << "         font-family:'EB Garamond', Georgia, serif; color:#fff5e6; text-align:center; }\n"
             << "  h1 { font-family:'Cinzel', serif; font-weight:800; color:#f4ead3; margin-bottom:5px; }\n"
             << "  .description { color:#a07b46; font-style:italic; font-size:1.2rem; margin-bottom:25px; }\n"
             << "  #map { max-width:900px; height:550px; margin:0 auto; border-radius:8px;\n"
             << "         border:2px solid #b89b6e;\n"
             << "         box-shadow:0 0 0 6px rgba(0,0,0,.25), 0 15px 40px rgba(0,0,0,.5); }\n"
             << "  .bulle { font-family:'EB Garamond', serif; color:#2a2018; font-size:1.1rem; }\n"
             << "  .bulle a { font-family:'Cinzel', serif; font-weight:bold; color:#9c6a3b; text-decoration:none; }\n"
             << "  .bulle a:hover { text-decoration:underline; }\n"
             << "</style>\n</head>\n<body>\n";

        flux << "  <h1>Chroniques de l'Aventure</h1>\n";
        flux << "  <p class=\"description\">Carte de localisation des etapes du recit</p>\n";

        flux << "  <div id=\"map\"></div>\n\n";

        flux << "  <script>\n"
             << "    var map = L.map('map').setView([10, 20], 3);\n\n"
             << "    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {\n"
             << "      maxZoom: 18,\n"
             << "      attribution: '© OpenStreetMap'\n"
             << "    }).addTo(map);\n\n";

        int index = 0;
        for (auto it = this->livre.begin(); it != this->livre.end(); ++it) {
            Page p = it.value();

            double lat = 0.0 + (index * 6.5);
            double lng = 0.0 + (index * 12.0);

            flux << "    L.marker([" << lat << ", " << lng << "]).addTo(map)\n"
                 << "      .bindPopup(\"<div class='bulle'><b>Etape " << QString::number(p.id()) << "</b><br>"
                 << p.titre().replace("\"", "\\\"") << "<br><br>\" +\n"
                 << "      \"<a href='page_" << QString::number(p.id()) << ".html'>Entrer dans ce chapitre</a></div>\");\n\n";

            index++;
        }

        flux << "  </script>\n</body>\n</html>\n";
        fichier.close();
    }
}
