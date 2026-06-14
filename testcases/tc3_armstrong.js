function isArmstrong(num) {
    const digits = String(num).split('');
    const n = digits.length;
    const sum = digits.reduce((acc, d) => acc + Math.pow(Number(d), n), 0);
    return sum === num;
}
console.log(isArmstrong(153)); // true
console.log(isArmstrong(370)); // true
console.log(isArmstrong(100)); // false
