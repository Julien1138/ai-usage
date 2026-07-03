# AI Usage

Application Pebble qui affiche les jauges d'utilisation de Claude.ai
(session de 5 h et limite hebdomadaire) directement sur la montre, avec le
temps restant avant réinitialisation sous chaque jauge.

## Fonctionnement

- Le téléphone (PebbleKit JS) envoie une requête sonde minimale
  (`max_tokens: 1`, ~1 token) sur `/v1/messages` avec un token OAuth
  Claude Code, et lit les en-têtes `anthropic-ratelimit-unified-*-utilization`
  (fraction 0-1) et `-reset` (epoch Unix) — même méthode que
  [Clawdmeter](https://github.com/HermannBjorgvin/Clawdmeter) et
  [claude-usage-stick](https://github.com/oauramos/claude-usage-stick).
  Ces en-têtes sont accessibles avec le scope `user:inference` des tokens
  `claude setup-token` (contrairement à l'endpoint `/api/oauth/usage`, qui
  exige `user:profile`).
- Sur la montre : en-tête « Claude usage » avec Clawd (la mascotte de
  Claude Code) en pixel art, puis une jauge par ligne — intitulé et
  pourcentage au-dessus de la barre, compte à rebours de réinitialisation
  en dessous. Couleurs claude.ai (barre corail `#D97757`). Sur les écrans
  ronds, la largeur de chaque jauge s'adapte à la corde du cercle à sa
  hauteur.
- Les dernières valeurs sont persistées sur la montre et affichées
  immédiatement à l'ouverture, le temps du rafraîchissement.

## Configuration

1. Sur un ordinateur avec Claude Code : `claude setup-token`, copier le token.
2. Dans l'app Pebble du téléphone : ouvrir les réglages de « AI Usage »
   (page générée localement par Clay), coller le token et choisir la
   période de mise à jour, puis Enregistrer. Le token reste sur le
   téléphone, il n'est jamais envoyé à la montre.

Les mises à jour n'ont lieu que lorsque l'application est ouverte sur la
montre : PebbleKit JS ne tourne pas en arrière-plan.

## Boutons

- **Haut / Bas** : défilement si toutes les jauges ne tiennent pas à l'écran.
- **Select** : rafraîchir immédiatement (sinon rafraîchissement automatique
  selon la période configurée, 5 min par défaut).

## Développement

```sh
pebble build                          # build pour toutes les plateformes
pebble install --emulator chalk       # tester sur l'émulateur rond
pebble install --phone <ip>           # installer sur le téléphone
```

Plateformes ciblées : aplite, basalt, chalk, diorite, emery, flint, gabbro.
La page de configuration utilise [@rebble/clay](https://www.npmjs.com/package/@rebble/clay)
(le fork Rebble de pebble-clay, compatible flint/gabbro).

## Structure

```
src/c/main.c         Cycle de vie, fenêtre, boutons, timer minute
src/c/gauges.c/.h    Modèle de données : jauges, statut, persistance
src/c/ui.c/.h        Rendu : titre, jauges adaptatives, défilement, resets
src/c/mascot.c/.h    Clawd en pixel art
src/c/comm.c/.h      AppMessage montre <-> téléphone
src/c/theme.h        Couleurs claude.ai (et équivalents noir & blanc)
src/pkjs/index.js    Côté téléphone : sonde API + en-têtes ratelimit, Clay
src/pkjs/config.js   Page de configuration (token, période de mise à jour)
```
