var t = require('./i18n');

module.exports = [
  {
    "type": "heading",
    "defaultValue": "AI Usage"
  },
  {
    "type": "text",
    "defaultValue": t('config_intro')
  },
  {
    "type": "section",
    "items": [
      {
        "type": "input",
        "messageKey": "ClaudeToken",
        "label": t('config_token_label'),
        "defaultValue": "",
        "attributes": {
          "placeholder": "sk-ant-oat01-...",
          "limit": 1024
        }
      },
      {
        "type": "text",
        "defaultValue": t('config_token_help')
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "select",
        "messageKey": "RefreshPeriod",
        "label": t('config_refresh_label'),
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
        "defaultValue": t('config_refresh_help')
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": t('config_submit')
  }
];
