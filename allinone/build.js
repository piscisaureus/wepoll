var path = require('path');

var fs = require('fs');
var included = {};

function lines(filename) {
  if (/[\/\\]/.test(filename))
    return fs.readFileSync(filename, 'utf8').split(/\r?\n/g);

  var PATH = [ '.', 'include', 'src' ];
  for (;;) {
    var dir = PATH.shift();
    try {
      return fs.readFileSync(dir + '/' + filename, 'utf8').split(/\r?\n/g);
    } catch (e) {
      if (PATH.length == 0)
        throw e;
    }
  }
}

function comment(s) {
  return ''; //'/* ' + s + '*/'
}

function include(line, filename) {
  var key = path.basename(filename).toLowerCase();
  if (included[key])
    return comment(line);
  console.error("Including: " + key);
  included[key] = true;
  return lines(filename);
}

var sys_included = {};
function include_sys(line, filename) {
  var key = path.basename(filename).toLowerCase();
  if (sys_included[key])
    return comment(line);

  sys_included[key] = true;
}

var source = [];
for (var i = 2; i < process.argv.length; i++) {
  var filename = process.argv[i];
  source = source.concat(include(null, filename));
}

var patterns = [
  {re : /^\s*#include\s*"([^"]*)".*$/, fn : include},
  {re : /^\s*#include\s*<([^"]*)>.*$/, fn : include_sys}
]

restart: for (var lno = 0; lno < source.length;) {
  for (var i in patterns) {
    var line = source[lno];
    var pattern = patterns[i];
    var match = pattern.re.exec(line);
    if (match) {
      var repl = pattern.fn.apply(null, match);
      if (repl != null && repl !== line) {
        source.splice.apply(source, [ lno, 1 ].concat(repl));
        continue restart;
      }
    }
  }
  lno++;
}

console.log(source.join('\n'));
