var Clay = require('@rebble/clay');
var clayConfig = require('./config');
var t = require('./i18n');
var clay = new Clay(clayConfig, null, { autoHandleEvents: false });

var MESSAGES_URL = 'https://api.anthropic.com/v1/messages';
var OAUTH_BETA = 'oauth-2025-04-20';
var PROBE_MODEL = 'claude-haiku-4-5-20251001';
var DEFAULT_REFRESH_MIN = 5;

// Seules jauges affichées : session de 5 h et limite hebdomadaire.
var BUCKETS = [
  { name: '5h', label: t('gauge_session_5h') },
  { name: '7d', label: t('gauge_week') }
];

var fetchInProgress = false;
var refreshTimer = null;

function getSettings() {
  try {
    return JSON.parse(localStorage.getItem('clay-settings')) || {};
  } catch (e) {
    return {};
  }
}

function getToken() {
  return (getSettings().ClaudeToken || '').trim();
}

function getRefreshMinutes() {
  var minutes = parseInt(getSettings().RefreshPeriod, 10);
  return (minutes >= 1 && minutes <= 120) ? minutes : DEFAULT_REFRESH_MIN;
}

function scheduleRefresh() {
  if (refreshTimer !== null) {
    clearInterval(refreshTimer);
  }
  refreshTimer = setInterval(fetchUsage, getRefreshMinutes() * 60 * 1000);
}

function sendToWatch(payload) {
  Pebble.sendAppMessage(payload, null, function(e) {
    console.log('Echec envoi AppMessage: ' + JSON.stringify(e));
  });
}

function sendStatus(message) {
  sendToWatch({ Status: message });
}

function sendGauges(gauges) {
  sendToWatch({
    GaugeLabels: gauges.map(function(g) { return g.label; }).join('|'),
    GaugePercents: gauges.map(function(g) { return g.percent; }).join('|'),
    GaugeResets: gauges.map(function(g) { return g.reset || 0; }).join('|')
  });
}

// Extrait les jauges des en-têtes anthropic-ratelimit-unified-<bucket>-* :
// utilization (fraction 0-1) et reset (epoch Unix).
function parseHeaderGauges(rawHeaders) {
  var gauges = [];
  BUCKETS.forEach(function(bucket) {
    var utilization = rawHeaders.match(new RegExp(
      'anthropic-ratelimit-unified-' + bucket.name + '-utilization:\\s*([0-9.]+)', 'i'));
    if (!utilization) {
      return;
    }
    var value = parseFloat(utilization[1]);
    if (isNaN(value)) {
      return;
    }
    var reset = rawHeaders.match(new RegExp(
      'anthropic-ratelimit-unified-' + bucket.name + '-reset:\\s*(\\d+)', 'i'));
    gauges.push({
      label: bucket.label,
      // Fraction 0-1 dans les en-têtes ; on tolère un pourcentage déjà exprimé.
      percent: value <= 1.5 ? Math.round(value * 100) : Math.round(value),
      reset: reset ? parseInt(reset[1], 10) : 0
    });
  });
  return gauges;
}

// Requête sonde minimale (~1 token) : seuls les en-têtes de réponse nous
// intéressent, présents même quand la requête est refusée pour rate limit.
function fetchUsage() {
  var token = getToken();
  if (!token) {
    sendStatus(t('status_no_token'));
    return;
  }
  if (fetchInProgress) {
    return;
  }
  fetchInProgress = true;

  var xhr = new XMLHttpRequest();
  xhr.open('POST', MESSAGES_URL);
  xhr.setRequestHeader('Authorization', 'Bearer ' + token);
  xhr.setRequestHeader('anthropic-version', '2023-06-01');
  xhr.setRequestHeader('anthropic-beta', OAUTH_BETA);
  xhr.setRequestHeader('Content-Type', 'application/json');
  xhr.timeout = 15000;

  xhr.onload = function() {
    fetchInProgress = false;
    var gauges = parseHeaderGauges(xhr.getAllResponseHeaders() || '');
    if (gauges.length > 0) {
      sendGauges(gauges);
    } else if (xhr.status === 401 || xhr.status === 403) {
      sendStatus(t('status_bad_token'));
    } else {
      sendStatus(t('status_no_gauges') + '\n(HTTP ' + xhr.status + ')');
    }
  };
  xhr.onerror = function() {
    fetchInProgress = false;
    sendStatus(t('status_network_error'));
  };
  xhr.ontimeout = function() {
    fetchInProgress = false;
    sendStatus(t('status_timeout'));
  };

  xhr.send(JSON.stringify({
    model: PROBE_MODEL,
    max_tokens: 1,
    messages: [{ role: 'user', content: '.' }]
  }));
}

Pebble.addEventListener('ready', function() {
  fetchUsage();
  scheduleRefresh();
});

Pebble.addEventListener('appmessage', function(e) {
  if (e.payload && e.payload.Refresh) {
    fetchUsage();
  }
});

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(clay.generateUrl());
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (!e || !e.response) {
    return;
  }
  try {
    // Persiste les réglages dans localStorage (clay-settings) ; le token
    // n'est jamais envoyé à la montre.
    clay.getSettings(e.response);
  } catch (err) {
    console.log('Erreur de config Clay: ' + err.message);
    return;
  }
  fetchUsage();
  scheduleRefresh();
});
