// This is a mess. I know.

var path = require('path');
var fs = require('fs');

// Parse command line options.

var files = [];
var includeDirs = [];
var stripGuardsEnabled = false;

process.argv
  .slice(2)
  .forEach((arg) => {
    let match;
    if (match = /^-I(.*)$/.exec(arg))
      includeDirs.push(match[1]);
    else if (arg === '--strip-guards')
      stripGuardsEnabled = true;
    else
      files.push(arg);
  });

var included = {};

function readFileWithPath(fileName, dirs) {
  if (/[/\\]/.test(fileName))
    return fs.readFileSync(fileName, 'utf8');

  for (let i = 0; i < dirs.length; i++) {
    var filePath = path.resolve(dirs[i], fileName);
    try {
      return fs.readFileSync(filePath, 'utf8');
    } catch (e) {
      // Ignore.
    }
  }

  var err = new Error('file not found: ' + fileName);
  err.code = 'ENOENT'
  throw err;
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

function lines(filename, strip) {
  var source = readFileWithPath(filename, ['.'].concat(includeDirs));
  if (strip) source = strip_guards(filename, source);
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
  return lines(filename, true);
}

function add(filename) {
  var key = path.basename(filename).toLowerCase();
  console.error('Adding: ' + key);
  included[key] = true;
  return lines(filename, stripGuardsEnabled);
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

for (var i = 0; i < files.length; i++) {
  var filename = files[i];
  source = source.concat(add(filename));
}

var patterns = [
  { re: /^\s*#include\s*"([^"]*)".*$/, fn: include },
  { re: /^\s*#include\s*<([^"]*)>.*$/, fn: include_sys }
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

source = source
  .map((line) => line.replace(/\s+$/, ''))
  .join('\n')
  .replace(/\n{3,}/g, '\n\n')
  .replace(/\n*$/, '\n');

process.stdout.write(source);
