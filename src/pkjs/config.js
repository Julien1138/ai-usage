module.exports = [
  {
    "type": "heading",
    "defaultValue": "AI Usage"
  },
  {
    "type": "text",
    "defaultValue": "Affiche vos jauges d'utilisation Claude sur votre Pebble."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "input",
        "messageKey": "ClaudeToken",
        "label": "Token OAuth Claude Code",
        "defaultValue": "",
        "attributes": {
          "placeholder": "sk-ant-oat01-...",
          "limit": 1024
        }
      },
      {
        "type": "text",
        "defaultValue": "Pour obtenir un token : installez Claude Code sur un ordinateur, lancez « claude setup-token » dans un terminal, puis copiez le token généré ici. Le token reste sur votre téléphone."
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "select",
        "messageKey": "RefreshPeriod",
        "label": "Période de mise à jour",
        "defaultValue": "5",
        "options": [
          { "label": "1 minute", "value": "1" },
          { "label": "2 minutes", "value": "2" },
          { "label": "5 minutes", "value": "5" },
          { "label": "10 minutes", "value": "10" },
          { "label": "15 minutes", "value": "15" },
          { "label": "30 minutes", "value": "30" }
        ]
      },
      {
        "type": "text",
        "defaultValue": "Les jauges ne sont mises à jour que lorsque l'application est ouverte sur la montre (appui Select pour forcer une mise à jour)."
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Enregistrer"
  }
];
