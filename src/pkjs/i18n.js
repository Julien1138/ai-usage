// Internationalisation côté téléphone : chaînes envoyées à la montre et
// page de réglages Clay, en anglais et en français. La langue vient de
// navigator.language ; l'anglais est la langue par défaut.

var MESSAGES = {
  en: {
    gauge_session_5h: '5h session',
    gauge_week: 'Week',
    gauge_overage_active: 'Now using usage credits',
    status_no_token: 'No token.\nOpen the settings in the Pebble app.',
    status_bad_token: 'Invalid or expired token.\nRun "claude setup-token" again.',
    status_no_gauges: 'Gauges not found',
    status_network_error: 'Network error',
    status_timeout: 'Request timed out',
    config_intro: 'Shows your Claude usage gauges on your Pebble.',
    config_token_label: 'Claude Code OAuth token',
    config_token_help: 'To get a token: install Claude Code on a computer, run "claude setup-token" in a terminal, then paste the generated token here. The token stays on your phone.',
    config_refresh_label: 'Refresh period',
    config_refresh_help: 'Gauges only refresh while the app is open on the watch (press Select to force a refresh).',
    config_submit: 'Save'
  },
  fr: {
    gauge_session_5h: 'Session 5h',
    gauge_week: 'Semaine',
    gauge_overage_active: 'Crédits d\'utilisation en cours',
    status_no_token: 'Pas de token.\nOuvrez les réglages dans l\'app Pebble.',
    status_bad_token: 'Token invalide ou expiré.\nRefaites « claude setup-token ».',
    status_no_gauges: 'Jauges introuvables',
    status_network_error: 'Erreur réseau',
    status_timeout: 'Délai dépassé',
    config_intro: 'Affiche vos jauges d\'utilisation Claude sur votre Pebble.',
    config_token_label: 'Token OAuth Claude Code',
    config_token_help: 'Pour obtenir un token : installez Claude Code sur un ordinateur, lancez « claude setup-token » dans un terminal, puis copiez le token généré ici. Le token reste sur votre téléphone.',
    config_refresh_label: 'Période de mise à jour',
    config_refresh_help: 'Les jauges ne sont mises à jour que lorsque l\'application est ouverte sur la montre (appui Select pour forcer une mise à jour).',
    config_submit: 'Enregistrer'
  }
};

function getLanguage() {
  var language = (typeof navigator !== 'undefined' && navigator.language) || '';
  return language.toLowerCase().indexOf('fr') === 0 ? 'fr' : 'en';
}

module.exports = function(key) {
  return MESSAGES[getLanguage()][key];
};
