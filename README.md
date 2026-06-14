# myjs — Custom JavaScript Runtime in C++

> A production-grade JavaScript runtime built in C++, powered by an embedded QuickJS engine.  
> Executes real ES2023 JavaScript — not a toy interpreter.

---

## Architecture

```
┌─────────────────────────┐
│    JavaScript Source    │  (file / stdin / -e flag)
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│     C++ Runtime Layer   │
│  • Argument parsing     │
│  • File / stdin input   │
│  • console.log binding  │
│  • Error reporting      │
│  • Promise queue drain  │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│   QuickJS Engine (C)    │
│  • Full ES2023 parser   │
│  • Bytecode compiler    │
│  • Garbage collector    │
│  • Async / Promise job  │
└────────────┬────────────┘
             │
             ▼
┌─────────────────────────┐
│         stdout          │
└─────────────────────────┘
```

---

## Features

### Core Language (ES2023)
| Feature | Support |
|---|---|
| `let` / `const` / `var` | ✅ |
| Arrow functions | ✅ |
| Template literals | ✅ |
| Destructuring (object + array) | ✅ |
| Spread / Rest operators | ✅ |
| Default parameters | ✅ |
| Optional chaining `?.` | ✅ |
| Nullish coalescing `??` | ✅ |
| Classes + Inheritance | ✅ |
| Generators / Iterators | ✅ |
| Async / Await | ✅ |
| Promises / `Promise.all` | ✅ |
| `for...of` / `for...in` | ✅ |
| `try` / `catch` / `finally` | ✅ |
| Closures | ✅ |
| Regex | ✅ |
| BigInt | ✅ |

### Built-in Objects
| Object | Methods |
|---|---|
| `Array` | `map`, `filter`, `reduce`, `find`, `findIndex`, `every`, `some`, `flat`, `sort`, `reverse`, `includes`, `join`, `slice`, `splice`, `push`, `pop`, ... |
| `String` | `split`, `trim`, `includes`, `startsWith`, `endsWith`, `repeat`, `replace`, `match`, `indexOf`, `toUpperCase`, `toLowerCase`, ... |
| `Object` | `keys`, `values`, `entries`, `assign`, `freeze`, `create`, ... |
| `Math` | `pow`, `sqrt`, `abs`, `floor`, `ceil`, `round`, `max`, `min`, `random`, `log`, `sin`, `cos`, ... |
| `Date` | `getFullYear`, `getMonth`, `getDate`, `getTime`, `now`, ... |
| `Map` / `Set` / `WeakMap` | ✅ Full support |
| `JSON` | `parse`, `stringify` |
| `Promise` | `all`, `race`, `allSettled`, `any`, `resolve`, `reject` |

### Runtime APIs
| API | Detail |
|---|---|
| `console.log` | Multi-arg, auto-serializes objects/arrays |
| `console.error` | Writes to stderr |
| `console.warn` | Tagged warn |
| `console.assert` | Assertion with message |
| `console.table` | JSON-formatted table |
| `process.exit(code)` | Clean exit with code |
| `process.version` | Reports `v20.0.0` |
| `setTimeout` / `clearTimeout` | Synchronous execution model |
| `setInterval` / `clearInterval` | Supported |

---

## Build

### Prerequisites
- GCC / Clang (C++17)
- CMake ≥ 3.14

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

Binary: `build/myjs`

---

## Usage

### Run a file
```bash
./myjs program.js
```

### Pipe from stdin (judge-friendly)
```bash
cat program.js | ./myjs
```

### Inline eval
```bash
./myjs -e 'console.log("Hello, World!")'
```

---

## Test Cases

### TC1 — Even / Odd
```js
let num = 7;
if (num % 2 === 0) console.log("Even");
else               console.log("Odd");
// Output: Odd
```

### TC2 — For Loop
```js
for (let i = 1; i <= 5; i++) console.log(i);
// Output: 1 2 3 4 5
```

### TC3 — Armstrong Number
```js
function isArmstrong(num) {
    const digits = String(num).split('');
    const n = digits.length;
    return digits.reduce((s, d) => s + Math.pow(Number(d), n), 0) === num;
}
console.log(isArmstrong(153)); // true
console.log(isArmstrong(100)); // false
```

### TC4 — Array Reverse
```js
console.log([1,2,3,4,5].reverse().join(', ')); // 5, 4, 3, 2, 1
```

### TC5 — String Palindrome
```js
function isPalindrome(s) {
    const c = s.toLowerCase().replace(/[^a-z0-9]/g,'');
    return c === c.split('').reverse().join('');
}
console.log(isPalindrome("racecar")); // true
```

### Closures
```js
function makeCounter(n=0) {
    return { inc: () => ++n, val: () => n };
}
```

### Classes & Inheritance
```js
class Animal { constructor(n,s){this.name=n;this.sound=s;} speak(){return `${this.name} says ${this.sound}`;} }
class Dog extends Animal { constructor(n){super(n,'Woof');} }
console.log(new Dog('Rex').speak()); // Rex says Woof
```

### Async / Await + Promises
```js
async function main() {
    const vals = await Promise.all([Promise.resolve(1), Promise.resolve(2)]);
    console.log(vals.join(',')); // 1,2
}
main();
```

---

## Repository Structure

```
build-your-js/
├── quickjs/          # Embedded QuickJS engine (C source)
│   ├── quickjs.c
│   ├── quickjs.h
│   ├── cutils.c/h
│   ├── dtoa.c/h
│   ├── libregexp.c/h
│   └── libunicode.c/h
├── src/
│   └── main.cpp      # C++ runtime wrapper
├── testcases/        # All test JS files
│   ├── tc1_even_odd.js
│   ├── tc2_for_loop.js
│   ├── tc3_armstrong.js
│   ├── tc4_array_reverse.js
│   ├── tc5_string_palindrome.js
│   ├── tc6_closures.js
│   ├── tc7_map_filter_reduce.js
│   ├── tc8_spread_rest.js
│   ├── tc9_promises.js
│   ├── tc10_classes.js
│   ├── tc11_destructuring.js
│   └── tc12_generators.js
├── CMakeLists.txt
└── README.md
```

---

## Why QuickJS?

| Property | Detail |
|---|---|
| Size | ~200KB compiled |
| Language | Pure C — zero dependencies |
| Standard | ES2023 compliant |
| License | MIT |
| Creator | Fabrice Bellard (creator of FFmpeg, QEMU) |

This is the same approach used by production software that embeds scripting: Lua in game engines, Python in Blender, V8 in Chrome. We embed QuickJS the same way.

---

## Innovation Points

1. **stdin support** — judges can pipe JS directly: `cat test.js | ./myjs`
2. **`-e` eval flag** — run one-liner JS without a file
3. **Full `console` object** — `log`, `error`, `warn`, `assert`, `table`, `dir`
4. **Promise queue drain** — `async/await` and `Promise.all` work correctly
5. **Clean error reporting** — stack traces printed to stderr, exit code 1 on error
6. **256MB heap + 4MB stack** — handles large inputs without OOM

---

*Built with QuickJS by Fabrice Bellard — MIT License*
