// This is a mess. I know.

const path = require('path');
const fs = require('fs');

// Parse command line options.

const files = [];
const includeDirs = [];
let stripGuardsEnabled = false;

process.argv.slice(2).forEach(arg => {
  let match;
  if ((match = /^-I(.*)$/.exec(arg))) {
    includeDirs.push(match[1]);
  } else if (arg === '--strip-guards') {
    stripGuardsEnabled = true;
  } else {
    files.push(arg);
  }
});

const included = {};

function readFileWithPath(fileName, dirs) {
  if (/[/\\]/.test(fileName)) {
    return fs.readFileSync(fileName, 'utf8');
  }

  for (let i = 0; i < dirs.length; i++) {
    const filePath = path.resolve(dirs[i], fileName);
    try {
      return fs.readFileSync(filePath, 'utf8');
    } catch (e) {
      // Ignore.
    }
  }

  const err = new Error('file not found: ' + fileName);
  err.code = 'ENOENT';
  throw err;
}

function strip_guards(filename, source) {
  const lead_comments_re = /^(\s*\/\*((?!\*\/)[\s\S])*\*\/)*\s*/;
  const trail_comments_re = /(\s*\/\*((?!\*\/)[\s\S])*\*\/)*\s*$/;
  const lead_guards_re = /^#ifndef\s+(\w+_H_)\s+#define\s+(\w+_H_)\s+/;
  const trail_guards_re = /#endif$/;

  // Strip leading and trailing comments and whitespace.
  source = source.replace(lead_comments_re, '');
  source = source.replace(trail_comments_re, '');

  // Find include guards.
  const lead_guards = lead_guards_re.exec(source);
  const trail_guards = trail_guards_re.exec(source);

  // Remove include guards, if found.
  if (lead_guards && trail_guards && lead_guards[1] == lead_guards[2]) {
    console.error('Stripping include guards from ' + filename);
    source = source.replace(lead_guards_re, '');
    source = source.replace(trail_guards_re, '');
  } else {
    console.error('No include guards found in ' + filename);
  }

  // Add back a trailing newline.
  source += '\n';

  return source;
}

function lines(filename, strip) {
  let source = readFileWithPath(filename, ['.'].concat(includeDirs));
  if (strip) {
    source = strip_guards(filename, source);
  }
  return source.split(/\r?\n/g);
}

function include(line, filename) {
  const key = path.basename(filename).toLowerCase();
  if (included[key]) {
    return ''; // Included earlier.
  }
  console.error('Including: ' + key);
  included[key] = true;
  return lines(filename, true);
}

function add(filename) {
  const key = path.basename(filename).toLowerCase();
  console.error('Adding: ' + key);
  included[key] = true;
  return lines(filename, stripGuardsEnabled);
}

const sys_included = {};
function include_sys(line, filename) {
  const key = path.basename(filename).toLowerCase();
  if (sys_included[key]) {
    return ''; // Included earlier.
  }
  sys_included[key] = true;
}

const declarations = {};
function declare(line) {
  const key = line.replace(/\s+/g, ' ').trim();
  if (declarations[key]) {
    return ''; // Declared earlier.
  }
  declarations[key] = true;
}

let source = [];

source = source
  .concat('/*')
  .concat(
    fs
      .readFileSync('LICENSE', 'utf8')
      .replace(/^\s+|\s+$/g, '')
      .split(/\r?\n/g)
      .map(line => ' * ' + line)
      .map(line => line.replace(/\s+$/, ''))
  )
  .concat(' */')
  .concat('');

for (let i = 0; i < files.length; i++) {
  const filename = files[i];
  source = source.concat(add(filename));
}

const patterns = [
  { re: /^\s*#include\s*"([^"]*)".*$/, fn: include },
  { re: /^\s*#include\s*<([^"]*)>.*$/, fn: include_sys },
  { re: /^\s*typedef\s+struct\s+\w+\s+\w+\s*;\s*$/, fn: declare }
];

restart: for (let lno = 0; lno < source.length; ) {
  for (const i in patterns) {
    const line = source[lno];
    const pattern = patterns[i];
    const match = pattern.re.exec(line);
    if (match) {
      const repl = pattern.fn.apply(null, match);
      if (repl != null && repl !== line) {
        source.splice.apply(source, [lno, 1].concat(repl));
        continue restart;
      }
    }
  }
  lno++;
}

source = source
  .map(line => line.replace(/\/\* clang-format (on|off) \*\//g, ''))
  .map(line => line.replace(/\s+$/, ''))
  .join('\n')
  .replace(/\n{3,}/g, '\n\n')
  .replace(/\n*$/, '\n');

process.stdout.write(source);
