# Publication sur le Pebble Appstore

Depuis 2026 il n'y a plus qu'un seul store : le **Pebble Appstore**
(apps.repebble.com), opéré par Rebble et financé par Core Devices. Une seule
soumission couvre donc Rebble et RePebble. Deux méthodes au choix.

## Méthode 1 : `pebble publish` (recommandée)

Depuis la racine du projet :

```sh
pebble build
pebble publish \
  --name "AI Usage" \
  --category Tools \
  --source https://github.com/Julien1138/ai-usage \
  --icon-small store-assets/icon_small_48.png \
  --icon-large store-assets/icon_large_144.png \
  --screenshots screenshots/*_gauges.png
```

Les fichiers de capture doivent commencer par le nom de la plateforme
(`basalt_...`, `chalk_...`) : c'est ainsi que `pebble publish` les associe
aux collections d'assets. Une capture est fournie pour chacune des
7 plateformes.

La commande demande une connexion (compte développeur Rebble) au premier
lancement, crée la fiche, téléverse le `.pbw` et peut générer les captures
manquantes par plateforme (`--gif-all-platforms`).

## Méthode 2 : portail développeur web

Sur <https://dev-portal.rebble.io> :

1. « Add a Watchapp » : titre `AI Usage`, URL du source
   `https://github.com/Julien1138/ai-usage`, e-mail de support, catégorie
   **Tools**, icônes `store-assets/icon_small_48.png` (48×48) et
   `store-assets/icon_large_144.png` (144×144).
2. « Add a release » : téléverser `build/ai-usage.pbw` (+ notes de version),
   puis « Publish » sur la release.
3. « Manage Asset Collections » : créer une collection par plateforme
   supportée avec description et sa capture `screenshots/<plateforme>_gauges.png` :
   - aplite / basalt / diorite / flint : 144×168
   - chalk : 180×180
   - emery : 200×228
   - gabbro (Pebble Round 2) : 260×260

## Textes proposés

**Description courte (EN)**

> See your Claude.ai usage gauges at a glance: 5-hour session and weekly
> limit, each with its percentage and time until reset. Configure once with
> a Claude Code OAuth token (`claude setup-token`); the token stays on your
> phone. Refresh interval is configurable, and Clawd the crab keeps an eye
> on your quota.

**Description courte (FR)**

> Vos jauges d'utilisation Claude.ai en un coup d'œil : session de 5 h et
> limite hebdomadaire, avec pourcentage et temps restant avant
> réinitialisation. Configuration unique avec un token OAuth Claude Code
> (`claude setup-token`) ; le token reste sur votre téléphone. Période de
> rafraîchissement réglable, et Clawd le crabe garde un œil sur votre quota.

**Notes de version 1.0.0**

> Première version : jauges Session 5h et Semaine, comptes à rebours de
> réinitialisation, couleurs claude.ai, écrans ronds et noir & blanc
> supportés, période de mise à jour configurable.

## Checklist des assets

| Asset | Fichier | Statut |
|---|---|---|
| `.pbw` | `build/ai-usage.pbw` (après `pebble build`) | ✅ |
| Icône 48×48 | `store-assets/icon_small_48.png` | ✅ |
| Icône 144×144 | `store-assets/icon_large_144.png` | ✅ |
| Icône de menu (embarquée) | `resources/images/menu_icon.png` | ✅ |
| Captures (une par plateforme, 7) | `screenshots/<plateforme>_gauges.png` | ✅ |
| Compte développeur Rebble | à créer/connecter sur dev-portal.rebble.io | ⬜ |
