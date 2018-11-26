const exec = require('child_process').execSync;
const resolve = require('path').resolve;

const getSHA = s => s.match(/^\s*([0-9a-f]{40})\s*$/i)[1];

const inherit = { stdio: 'inherit' };
const utf8 = { encoding: 'utf8' };

let stdout;

const version = process.argv[2];
if (!/^\d+\.\d+\.\d+$/.test(version))
  throw new Error('usage: node release.js {version}');

const distTag = `v${version}`;

const projectRootDir = resolve(__dirname, '..');
process.chdir(projectRootDir);

stdout = exec('git.exe status --porcelain -uno', utf8);
if (stdout.match(/\S/))
  throw new Error('git index or working directory not clean');

exec('cmake.exe --build . --target dist', inherit);

stdout = exec('git rev-parse --verify HEAD', utf8);
const sourceCommit = getSHA(stdout);

stdout = exec('git write-tree', utf8);
const sourceTree = getSHA(stdout);

stdout = exec('git log --pretty="%D"', utf8);
const previousTag = stdout.match(/tag: ([^\s,;]+)/)[1];

stdout = exec(`git rev-list -n 1 ${previousTag}`, utf8);
const previousTagCommit = getSHA(stdout);

exec('git add -f dist/');

stdout = exec('git write-tree --prefix=dist/', utf8);
const distTree = getSHA(stdout);

exec('git reset');

stdout = exec(`git commit-tree -S ${distTree} ` +
              `-p ${previousTagCommit} ` +
              `-p ${sourceCommit} ` +
              `-m "version ${version}"`, utf8);
const distCommit = getSHA(stdout);

stdout = exec(`git commit-tree -S ${sourceTree} ` +
              `-p ${sourceCommit} ` +
              `-p ${distCommit} ` +
              `-m "dist: merge release tag ${distTag}"`, utf8);
const mergeCommit = getSHA(stdout);

exec(`git diff --stat ${previousTagCommit}..${distCommit}`, inherit);

console.log('');
console.log(`Previous release tag: ${previousTag}`);
console.log(`New release tag ${distTag}`);

exec(`git tag ${distTag} ${distCommit} ` +
     `-sm "version ${version}"`, inherit);

exec(`git branch -f dist ${distCommit}`, inherit);
exec(`git update-ref HEAD ${mergeCommit}`, inherit);

exec('git log --graph --decorate -n 3', inherit);
