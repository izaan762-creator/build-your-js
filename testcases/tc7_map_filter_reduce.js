const nums = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
const result = nums
    .filter(n => n % 2 === 0)
    .map(n => n * n)
    .reduce((acc, n) => acc + n, 0);
console.log(result); // 220
