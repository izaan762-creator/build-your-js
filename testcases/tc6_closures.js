function makeCounter(start = 0) {
    let count = start;
    return {
        increment: () => ++count,
        decrement: () => --count,
        value: () => count
    };
}
const c = makeCounter(10);
console.log(c.increment()); // 11
console.log(c.increment()); // 12
console.log(c.decrement()); // 11
console.log(c.value());     // 11
