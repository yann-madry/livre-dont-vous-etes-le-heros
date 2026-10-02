# Livre dont vous êtes le héros – éditeur et lecteur

Application de bureau en **C++ / Qt Widgets** pour écrire puis jouer un livre-jeu. L'auteur crée des pages reliées par des choix, l'application vérifie que l'histoire tient debout, puis permet d'y jouer ou de l'exporter en site web.

Projet réalisé en équipe de trois en 1ʳᵉ année de BUT Informatique (IUT Lyon 1, site de Bourg-en-Bresse), SAÉ 2.01, juin 2026.

## Fonctionnalités

- **Éditeur de pages** en texte riche : police, couleur, gras, italique, souligné, surlignage, listes, liens et images.
- **Pages et choix**
  - une page est normale, de victoire ou de défaite ;
  - un choix mène à une autre page et peut modifier les points de vie, l'expérience ou donner un objet ;
  - un choix peut être soumis à une condition : posséder un objet, ou être déjà passé par une page.
- **Mode lecture** : on joue le livre dans une fenêtre dédiée, avec points de vie, expérience et inventaire.
- **Vérification de cohérence** : l'application signale les pages inaccessibles et les culs-de-sac (pages sans choix qui ne sont ni une victoire ni une défaite).
- **Sauvegarde** au format JSON, avec sauvegarde automatique.
- **Export** en site web (une page HTML par page du livre, plus une carte interactive Leaflet), en HTML, en PDF, ou impression.

Un livre d'exemple est fourni dans `Livre/` : *La Caverne Maudite*, avec son export en site web.

## Organisation du code

| Fichiers | Rôle |
|---|---|
| `page`, `choix`, `condition`, `EtatJoueur.h` | Modèle : contenu d'une page, choix, conditions, état du joueur |
| `livre` | Ensemble des pages (`QMap` indexée par identifiant), export en site web |
| `mainwindow` | Fenêtre d'édition, sauvegarde, exports, vérification de cohérence |
| `fenetrerun` | Fenêtre de lecture (mode jeu) |

## Compiler

Prérequis : Qt 6 (modules Widgets et PrintSupport), CMake 3.16 ou plus, un compilateur C++17.

Le plus simple est d'ouvrir `CMakeLists.txt` dans Qt Creator. En ligne de commande :

```bash
cmake -B build
cmake --build build
```

## Équipe

**Yann Madry**, **Tristan Muller** et **Thomas Bonnefoy**.

### Ma partie

- **Classes `Page` et `Livre`** : le cœur du modèle. `Livre` stocke les pages dans une `QMap` et gère la page de départ, l'ajout et la suppression.
- **Sauvegarde JSON** : méthodes `versJson` et `depuisJson` de `Page` et `Choix`, pour enregistrer puis reconstruire un livre à l'identique.
- **Vérification de cohérence** (`verifierCoherenceLivre`) : parcours en profondeur itératif du livre vu comme un graphe, avec une pile et la liste des pages déjà visitées pour ne pas tourner en rond.
- Tests du modèle et histoire d'exemple.

## Ce que j'ai appris

- Séparer le modèle de l'interface pour que chaque classe ait un seul rôle.
- Sérialiser des objets imbriqués en JSON (tableau de choix, type de page énuméré).
- Réutiliser un algorithme de graphe vu en cours sur un vrai problème.
