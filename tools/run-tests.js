
const spawn = require('child_process').spawn;
const basename = require('path').basename;

const test_exes = process.argv.slice(2);
run_tests(test_exes);

function run_tests(test_exes, num = 0, fail_count = 0) {
  if (test_exes.length <= num)
    return done(test_exes, fail_count);

  const test_exe = test_exes[num];
  const test_name = basename(test_exe, '.exe');

  console.log('(%d/%d) %s...', (num + 1), test_exes.length, test_name);

  const child = spawn(test_exe, [], { encoding: 'utf8' });

  let out = '';
  child.stdout.on('data', (data) => out += data);
  child.stderr.on('data', (data) => out += data);

  child.on('exit', (code) => {
    if (code === 0) {
      console.log('  PASS');
    } else {
      console.log('  FAIL\n' + out);
      fail_count++;
    }

    run_tests(test_exes, num + 1, fail_count);
  });
}

function done(test_exes, fail_count) {
  const pass_count = test_exes.length - fail_count;
  console.log('  DONE - %d PASSED, %d FAILED', pass_count, fail_count);

  process.exit(fail_count == 0 ? 0 : 1);
}

