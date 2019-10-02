export default function getUrlParam(param) {
  const params = {};
  window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, function(m,key,value) {
    params[key] = decodeURIComponent(value);
  });

  return params[param] || null;
}
