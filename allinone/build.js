// This is a mess. I know.

var path = require('path');
var fs = require('fs');

var included = {};

function load(filename) {
  if (/[\/\\]/.test(filename))
    return fs.readFileSync(filename, 'utf8');

  var PATH = ['.', 'include', 'src'];
  for (;;) {
    var dir = PATH.shift();
    try {
      return fs.readFileSync(dir + '/' + filename, 'utf8');
    } catch (e) {
      if (PATH.length == 0)
        throw e;
    }
  }
}

function strip_guards(filename, source) {
  var lead_comments_re = /^(\s*\/\*((?!\*\/)[\s\S])*\*\/)*\s*/;
  var trail_comments_re = /(\s*\/\*((?!\*\/)[\s\S])*\*\/)*\s*$/;
  var lead_guards_re = /^#ifndef\s+(\w+)\s+#define\s+(\w+)\s+/;
  var trail_guards_re = /#endif$/;

  // Strip leading and trailing comments and whitespace.
  source = source.replace(lead_comments_re, '');
  source = source.replace(trail_comments_re, '');

  // Find include guards.
  var lead_guards = lead_guards_re.exec(source);
  var trail_guards = trail_guards_re.exec(source);

  // Remove include guards, if found.
  if (lead_guards && trail_guards && lead_guards[1] == lead_guards[2]) {
    console.error('Stripping include guards: ' + filename);
    source = source.replace(lead_guards_re, '');
    source = source.replace(trail_guards_re, '');
  }

  // Add back a trailing newline.
  source += '\n';

  return source;
}

function lines(filename) {
  var source = load(filename);
  source = strip_guards(filename, source);
  return source.split(/\r?\n/g);
}

function comment(s) {
  return '';  //'/* ' + s + '*/'
}

function include(line, filename) {
  var key = path.basename(filename).toLowerCase();
  if (included[key])
    return comment(line);
  console.error('Including: ' + key);
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

source = source.concat('/*')
             .concat(fs.readFileSync('LICENSE', 'utf8')
                         .replace(/^\s+|\s+$/g, '')
                         .split(/\r?\n/g)
                         .map(function(s) {
                           return ' * ' + s;
                         })
                         .map(function(s) {
                           return s.replace(/\s+$/, '');
                         }))
             .concat(' */')
             .concat('');

for (var i = 2; i < process.argv.length; i++) {
  var filename = process.argv[i];
  source = source.concat(include(null, filename));
}

var patterns = [
  {re: /^\s*#include\s*"([^"]*)".*$/, fn: include},
  {re: /^\s*#include\s*<([^"]*)>.*$/, fn: include_sys}
]

restart: for (var lno = 0; lno < source.length;) {
  for (var i in patterns) {
    var line = source[lno];
    var pattern = patterns[i];
    var match = pattern.re.exec(line);
    if (match) {
      var repl = pattern.fn.apply(null, match);
      if (repl != null && repl !== line) {
        source.splice.apply(source, [lno, 1].concat(repl));
        continue restart;
      }
    }
  }
  lno++;
}

var emptyOk = false;
for (var i = 0; i < source.length; i++) {
  var line = source[i].replace(/\s+$/, '');
  if (line === '') {
    if (emptyOk && i < source.length - 1)
      process.stdout.write('\n');
    emptyOk = false;
  } else {
    process.stdout.write(line + '\n');
    emptyOk = true;
  }
}
