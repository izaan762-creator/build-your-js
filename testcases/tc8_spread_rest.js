function sum(...nums) {
    return nums.reduce((a, b) => a + b, 0);
}
const a = [1, 2, 3];
const b = [4, 5, 6];
const merged = [...a, ...b];
console.log(merged.join(', '));   // 1, 2, 3, 4, 5, 6
console.log(sum(...merged));      // 21
