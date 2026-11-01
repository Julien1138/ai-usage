# Veille : accès à l'usage des abonnements IA

Suivi daté des moyens de consulter l'usage d'un abonnement (pas de la
facturation API) chez Anthropic, OpenAI et Mistral. Ajouter une nouvelle
section datée à chaque vérification plutôt que de réécrire les précédentes.

Statut des sources : **officiel** (doc ou code du fournisseur),
**secondaire** (blog, guide tiers, issue GitHub).

## 2026-10-14

Contexte : on m'a signalé « Oauth for claude doesn't exist anymore. Use the
api ». Recherches web uniquement, rien testé.

### Synthèse

| Fournisseur | Endpoint web public pour l'usage d'abonnement | Voie programmatique la plus officielle | Accessible directement depuis le téléphone |
| ----------- | --------------------------------------------- | --------------------------------------- | ------------------------------------------ |
| Anthropic   | Non                                           | JSON de la status line Claude Code      | Non (relais via ordinateur)                |
| OpenAI      | Non                                           | `codex app-server` → `account/rateLimits/read` | Non (relais via ordinateur)         |
| Mistral     | Non (Le Chat sans compteur)                   | Admin API `/usage` (montant consommé)   | Oui, mais clé admin                        |

### Anthropic (Claude Pro/Max)

- **Restriction des tokens OAuth** (secondaire) :
  - 2026-01-09 : blocage serveur des tokens d'abonnement hors Claude Code
    (« This credential is only authorized for use with Claude Code »).
  - 2026-02-19 : les CGU Claude Code interdisent l'usage des tokens OAuth
    Free/Pro/Max dans un autre produit.
  - ~2026-02-20 : tokens `claude setup-token` (`sk-ant-oat01-`) rejetés selon
    l'issue #28091 (« OAuth authentication is currently not supported »).
  - 2026-04-04 : extension annoncée à tous les outils tiers.
  - Méthode actuelle de l'app (sonde `/v1/messages` + `anthropic-beta:
    oauth-2025-04-20`) concernée. État technique à vérifier avec un vrai
    token.
- **Interfaces humaines** (officiel) : claude.ai Settings → Usage ;
  `/usage`, `/status` dans Claude Code.
- **Status line Claude Code** (secondaire, ajout attribué à la v2.1.80) :
  `rate_limits.five_hour` et `rate_limits.seven_day`, chacun avec
  `used_percentage` (0-100) et `resets_at` (epoch). Présent seulement pour
  Pro/Max et après la première réponse de la session. Absences signalées
  (#40094, #45133). Limites hebdomadaires par modèle non exposées.
- **Admin API** (officiel) : `/v1/organizations/usage_report/messages`,
  `/v1/organizations/cost_report`, clé `sk-ant-admin`. Organisations API
  uniquement, ne couvre pas les abonnements.
- Les en-têtes `anthropic-ratelimit-*` obtenus avec une clé API
  reflètent les limites API, pas l'abonnement.

### OpenAI (ChatGPT Plus/Pro, Codex)

- **Interfaces humaines** (officiel) : chatgpt.com/codex/settings/usage ;
  `/status` dans Codex.
- **`codex app-server`** (secondaire ; commit upstream #5302) : méthode
  JSON-RPC `account/rateLimits/read` après `initialize`. Réponse
  `rateLimits.primary` (5 h) et `rateLimits.secondary` (hebdo), avec
  `usedPercent` et `resetsAt`. Utilise la connexion ChatGPT locale. Durée
  de la fenêtre primaire à lire dans la réponse (sources divergentes).
- **Admin API** (officiel) : `/v1/organization/costs`, clé admin
  (Owners). Facturation API uniquement.
- Pro : pas de limite 5 h selon certaines sources, limite hebdomadaire
  maintenue.

### Mistral (Le Chat, Vibe)

- **Le Chat Pro** (secondaire) : « illimité » sous fair-use non publié
  (~6× Free). Aucun compteur d'usage trouvé.
- **Admin API** (officiel) :
  `GET https://console.mistral.ai/api/admin/usage`, en-tête `x-api-key`
  (clé admin créée dans le Backoffice). Consommation mensuelle par
  catégorie, dont `vibe_usage`. `/spend-limit` pour le plafond mensuel.
- **API d'inférence** (officiel/secondaire) : en-têtes de rate limit
  (`X-RateLimit-Remaining` selon la doc, `ratelimitbysize-*` selon un
  guide tiers). Limites API, pas l'abonnement.
- Aucune commande `vibe` affichant le budget restant trouvée.

### Sources

- [GIGAZINE – Anthropic bans third-party subscription authentication](https://gigazine.net/gsc_news/en/20260220-anthropic-third-party-block/)
- [GitHub #28091 – OAuth tokens disabled for third-party apps](https://github.com/anthropics/claude-code/issues/28091)
- [WinBuzzer – Anthropic Bans Claude Subscription OAuth](https://winbuzzer.com/2026/02/19/anthropic-bans-claude-subscription-oauth-in-third-party-apps-xcxwbn/)
- [Decode the Future – Anthropic Blocks Third-Party Tools](https://decodethefuture.org/en/anthropic-blocks-third-party-tools/)
- [Use Claude Code with your Pro or Max plan](https://support.claude.com/en/articles/11145838-use-claude-code-with-your-pro-or-max-plan)
- [wmedia.es – Usage limit in status line](https://wmedia.es/en/tips/claude-code-usage-limit-status-line)
- [GitHub #40094](https://github.com/anthropics/claude-code/issues/40094) · [GitHub #45133](https://github.com/anthropics/claude-code/issues/45133)
- [Per-model weekly limits request](https://claudeissues.com/issue/73770-statusline-expose-per-model-weekly-rate-limits-opus-sonnet-fable-to-the-status-l)
- [Anthropic Usage and Cost API](https://platform.claude.com/docs/en/build-with-claude/usage-cost-api)
- [Ian Paterson – Tracking Claude/Codex/Gemini quotas](https://ianlpaterson.com/blog/tracking-claude-codex-gemini-quotas-from-one-script/)
- [Codex commit #5302 – read rate limits API](https://forge.lthn.ai/core/core-agent-ide/commit/5c680c6587e5a2c3e65b8b7b45c9df52c69bc8a6)
- [OpenAI Help – Managing usage in Codex](https://help.openai.com/en/articles/20001516-managing-usage-with-gpt-6-astra-in-work-and-codex)
- [Harness – OpenAI cost integration](https://developer.harness.io/cloud-cost-management/integrations/ai-providers/openai.md)
- [Mistral Admin API – Usage metrics](https://docs.mistral.ai/admin/admin-api/usage-metrics)
- [Mistral – Usage and limits](https://docs.mistral.ai/admin/billing-usage/usage-limits)
- [ailimit.watch – Le Chat limits](https://ailimit.watch/tools/mistral/)

## 2026-11-01

Recherches web (WebSearch et WebFetch), rien testé avec de vrais comptes.
`docs.mistral.ai` et `developers.openai.com` n'étaient pas joignables depuis
la session (domaines hors liste autorisée) : ces deux sources officielles
n'ont pas pu être relues directement.

### Changements depuis le 2026-10-14

- **Status line Claude Code : passage de secondaire à officiel.** La doc
  officielle (page « Customize your status line ») documente désormais
  `rate_limits.five_hour` et `rate_limits.seven_day` avec `used_percentage`
  (0-100) et `resets_at` (epoch en secondes). Elle ajoute un objet
  `rate_limits.spend_limit` (`used_percentage`, `resets_at`, et
  `used_usd`/`limit_usd`/`period`), pertinent seulement derrière une
  « Claude apps gateway » (v2.1.251+ puis v2.1.284+), et un champ
  `prompt_cache` (v2.1.251+). Elle indique aussi que la status line est
  rafraîchie quand une fenêtre atteint son `resets_at`. Non vérifié ici : la
  disponibilité exacte selon le plan et les absences signalées (#40094,
  #45133).
- **Politique OAuth : désormais confirmée par une source officielle.** La page
  officielle « Legal and compliance » de Claude Code indique que
  l'authentification OAuth est réservée aux abonnés et à l'usage de Claude
  Code et des applications natives Anthropic ; les développeurs (Agent SDK
  inclus) doivent utiliser une clé API ; il est interdit de router des
  requêtes via des identifiants Free/Pro/Max/Ultra pour le compte
  d'utilisateurs, ou de collecter/intermédier des identifiants claude.ai.
  Un utilisateur peut en revanche se connecter au binaire Claude Code non
  modifié avec son propre abonnement. Rien sur `claude setup-token` dans cette
  page : son statut technique reste non vérifié.
- **Codex `account/rateLimits/read` : précisions** (secondaire, via PR #5302 et
  un miroir du README). Méthode sans paramètre, réponse `rateLimits.primary`
  avec `usedPercent`, `windowDurationMins`, `resetsAt`, `secondary`
  possiblement nul, notification `account/rateLimits/updated`, refus si
  l'authentification n'est pas de type ChatGPT, schéma annoncé instable. Le
  README officiel consulté (branche main) ne mentionne pas ces méthodes dans
  ce que j'ai pu lire : à revérifier, l'extrait étant possiblement
  incomplet.
- **Mistral** : des sources secondaires indiquent un renommage de Le Chat en
  « Vibe » (mai 2026), non confirmé par une source officielle lue ; l'Admin
  API est décrite (résultat de recherche sur docs.mistral.ai) comme exposant
  aussi des analytics produit Le Chat/Vibe (par ex. `vibe/usage/by_organization`)
  et un quota mensuel partagé entre Studio, API et Vibe Code. Non relu
  directement.
- Pas d'autre changement constaté : aucun endpoint public d'usage
  d'abonnement trouvé chez les trois fournisseurs.

### Synthèse

| Fournisseur | Endpoint web public pour l'usage d'abonnement | Voie programmatique la plus officielle | Accessible directement depuis le téléphone |
| ----------- | --------------------------------------------- | --------------------------------------- | ------------------------------------------ |
| Anthropic   | Non trouvé                                    | JSON de la status line Claude Code (maintenant documenté) | Non (relais via ordinateur) |
| OpenAI      | Non trouvé                                    | `codex app-server` → `account/rateLimits/read` (secondaire) | Non (relais via ordinateur) |
| Mistral     | Non (aucun compteur Le Chat trouvé)           | Admin API `/usage` (montant consommé) | Oui, mais clé admin (non revérifié) |

### Anthropic (Claude Pro/Max)

- **Politique des tokens OAuth** (officiel pour la politique actuelle,
  secondaire pour la chronologie 2026-01-09 / 02-19 / 04-04 déjà notée) :
  usage hors Claude Code et apps natives non autorisé. Une source secondaire
  rapporte une facturation en « Extra Usage » pour les outils tiers depuis le
  4 avril ; une autre évoque un assouplissement pour l'usage CLI : affirmations
  non vérifiées.
- **Interfaces humaines** (officiel) : claude.ai Settings → Usage ; `/usage`,
  `/status`.
- **Status line** (officiel) : voir « Changements ». Limites hebdomadaires par
  modèle toujours non vues dans la doc.
- **Admin API** (non revérifié, officiel en 2026-10-14) : organisations API
  uniquement.

### OpenAI (ChatGPT Plus/Pro, Codex)

- **Interfaces humaines** : non revérifié (page inaccessible), inchangé.
- **`codex app-server`** (secondaire) : voir « Changements ».
- **Admin API `/v1/organization/costs`** : non revérifié.

### Mistral (Le Chat, Vibe)

- **Admin API** (officiel, via extraits de recherche) : base
  `https://console.mistral.ai/api/admin`, en-tête `x-api-key`, usage de
  facturation et analytics Le Chat/Vibe, endpoints de plafond mensuel.
- **Limites** (officiel, extraits) : budget mensuel du plan partagé entre
  Studio, API et Vibe Code ; limites de débit API distinctes. Aucune limite
  chiffrée publiée pour Vibe (secondaire).
- Aucune commande `vibe` affichant le budget restant trouvée.

### Sources

- [Claude Code – Customize your status line](https://code.claude.com/docs/en/statusline)
- [Claude Code – Legal and compliance](https://code.claude.com/docs/en/legal-and-compliance)
- [GitHub #45133](https://github.com/anthropics/claude-code/issues/45133)
- [wmedia.es – Usage limit in status line](https://wmedia.es/en/tips/claude-code-usage-limit-status-line)
- [AlternativeTo – Anthropic bans subscription auth for third-party use](https://alternativeto.net/news/2026/2/anthropic-officially-bans-using-subscription-authentication-for-third-party-claude-use)
- [WinBuzzer – Anthropic Bans Claude Subscription OAuth](https://winbuzzer.com/2026/02/19/anthropic-bans-claude-subscription-oauth-in-third-party-apps-xcxwbn/)
- [Codex PR #5302 – read rate limits API](https://github.com/openai/codex/pull/5302)
- [Codex app-server README](https://github.com/openai/codex/blob/main/codex-rs/app-server/README.md)
- [Mistral Admin API – Usage metrics](https://docs.mistral.ai/admin/admin-api/usage-metrics)
- [Mistral – Usage and limits](https://docs.mistral.ai/admin/billing-usage/usage-limits)
- [Mistral Help – API rate limits](https://help.mistral.ai/en/articles/698531-why-am-i-hitting-api-rate-limits-and-how-do-i-increase-them)
