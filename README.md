# Livre dont vous êtes le héros – éditeur et lecteur

Application de bureau en **C++ / Qt** pour écrire un livre-jeu, vérifier qu'il tient debout, y jouer et le publier en site web.

**[Lire le livre d'exemple exporté par l'application](https://yann-madry.github.io/livre-dont-vous-etes-le-heros/)**

## Contexte du projet

| | |
|---|---|
| Cadre | SAÉ 2.01, BUT Informatique 1ʳᵉ année, IUT Lyon 1 – site de Bourg-en-Bresse |
| Date et durée | Juin 2026, une semaine |
| Équipe | 3 personnes |
| Technologies | C++17, Qt 6 (Widgets), CMake, JSON, HTML / CSS, Leaflet |

Le sujet : réaliser une application de bureau permettant de créer des livres « dont vous êtes le héros », ces récits où le lecteur choisit la suite de l'histoire à chaque page.

## Présentation du projet

L'application couvre tout le cycle de vie d'un livre-jeu :

1. **Écrire** : l'auteur rédige ses pages en texte riche et les relie par des choix.
2. **Vérifier** : l'application détecte les pages qu'aucun chemin n'atteint et les impasses oubliées.
3. **Jouer** : un mode lecture fait vivre l'histoire avec points de vie, expérience et inventaire.
4. **Publier** : le livre s'exporte en site web, avec une carte interactive des chapitres.

## Objectifs pédagogiques

Cette SAÉ mobilise principalement trois compétences du BUT Informatique :

- **Réaliser un développement d'application** : concevoir et coder une application complète en orienté objet, avec une interface graphique.
- **Optimiser des applications** : choisir les bonnes structures de données et appliquer un algorithme de graphe à un problème réel.
- **Travailler dans une équipe informatique** : se répartir le travail, partager le code avec Git et tenir un suivi des tâches.

## Travail réalisé

- Une application complète, compilée avec CMake et Qt 6.
- Un livre d'exemple, *La Caverne Maudite*, fourni au format JSON et publié en ligne via l'export web.
- Une fiche de suivi individuelle décrivant les tâches, le temps passé et les choix techniques.

## Fonctionnalités

- **Éditeur de pages** en texte riche : police, couleur, gras, italique, souligné, surlignage, listes, liens, images.
- **Pages et choix**
  - une page est normale, de victoire ou de défaite ;
  - un choix mène à une autre page et peut modifier les points de vie, l'expérience ou donner un objet ;
  - un choix peut dépendre d'une condition : posséder un objet, ou être déjà passé par une page.
- **Mode lecture** avec points de vie, expérience et inventaire.
- **Vérification de cohérence** : signalement des pages inaccessibles et des culs-de-sac.
- **Sauvegarde** au format JSON, avec sauvegarde automatique.
- **Export** en site web (une page HTML par chapitre et une carte interactive), en HTML, en PDF, ou impression.

## Architecture du projet

```
.
├── README.md
├── LICENSE
├── fiche-de-suivi-yann-madry.pdf   Suivi de mes tâches et description technique
├── code/                           Projet Qt (CMake)
│   ├── CMakeLists.txt
│   ├── page, choix, condition      Modèle : pages, choix et conditions
│   ├── EtatJoueur.h                État du lecteur : points de vie, expérience, objets
│   ├── livre                       Ensemble des pages, export en site web
│   ├── mainwindow                  Fenêtre d'édition
│   ├── fenetrerun                  Fenêtre de lecture
│   ├── Icones/                     Icônes de l'interface
│   └── Livre/                      Livre d'exemple au format JSON
└── docs/                           Livre d'exemple exporté en site web (publié en ligne)
```

Le modèle (pages, choix, conditions, livre) est séparé de l'interface (fenêtres d'édition et de lecture) : chaque classe a un seul rôle, et le modèle peut être testé sans interface.

## Organisation du travail

Projet mené à trois : **Yann Madry**, **Tristan Muller** et **Thomas Bonnefoy**. Le code était partagé sur le GitLab de l'IUT, avec des réunions d'équipe régulières et une fiche de suivi par personne.

### Ma contribution

J'ai pris en charge **le cœur de l'application : le modèle de données et sa fiabilité**.

- **Classes `Page` et `Livre`** : structure centrale du programme. `Livre` stocke toutes les pages dans une `QMap` indexée par identifiant et gère la page de départ, l'ajout et la suppression.
- **Sauvegarde JSON** : méthodes `versJson` et `depuisJson` de `Page` et `Choix`, qui enregistrent puis reconstruisent un livre à l'identique.
- **Vérification de cohérence** : la fonction `verifierCoherenceLivre` parcourt le livre comme un graphe pour repérer ce qu'un auteur ne voit pas à l'œil nu.
- **Tests du modèle** et écriture du livre d'exemple.

### Répartition

| Partie | Responsable |
|---|---|
| Modèle de données, sauvegarde JSON, vérification de cohérence | Yann Madry |
| Éditeur de texte riche, export HTML | Tristan Muller |
| Mode lecture (points de vie, expérience, objets), carte interactive | Thomas Bonnefoy |

## Documents

| Document | Contenu |
|---|---|
| [`fiche-de-suivi-yann-madry.pdf`](fiche-de-suivi-yann-madry.pdf) | Mes tâches, le temps passé et la description technique de ma partie |
| [`docs/`](docs/) | Livre d'exemple exporté par l'application, [consultable en ligne](https://yann-madry.github.io/livre-dont-vous-etes-le-heros/) |

## Implémentation

### Points techniques

- **Structures de données Qt** : `QMap<int, Page>` pour retrouver une page par son identifiant, `QVector<Choix>` pour les choix d'une page.
- **Parcours en profondeur itératif** : une pile (un `QVector` utilisé avec `takeLast()`) et la liste des pages déjà visitées, pour explorer tous les chemins sans tourner en boucle. Les pages jamais atteintes et les pages sans issue sont signalées à l'auteur.
- **Sérialisation JSON** : objets imbriqués (`QJsonObject`, `QJsonArray`) et conversion d'un type énuméré (`enum class`) en donnée brute.
- **Export web** : génération d'une page HTML par chapitre, en UTF-8 via `QFile` et `QTextStream`, et d'une carte interactive avec la bibliothèque Leaflet.

### Compiler et lancer

Prérequis : Qt 6 (modules Widgets et PrintSupport), CMake 3.16 ou plus, un compilateur C++17.

```bash
cmake -S code -B build
cmake --build build
```

On peut aussi ouvrir `code/CMakeLists.txt` dans Qt Creator. Pour essayer l'application, ouvrir le livre d'exemple `code/Livre/La_Caverne_Maudite.json`.

## Suite du projet

- Afficher le graphe des pages directement dans l'éditeur, pour voir la structure de l'histoire d'un coup d'œil.
- Intégrer au dépôt des tests automatisés du modèle (Qt Test).
- Proposer des versions prêtes à l'emploi pour Windows et macOS.

## Licence

Ce projet est sous licence [Creative Commons BY-NC 4.0](LICENSE) : vous pouvez le réutiliser et l'adapter en citant les auteurs, mais pas à des fins commerciales.
