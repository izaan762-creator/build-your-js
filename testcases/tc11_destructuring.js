const { name, age, address: { city } = {} } = {
    name: 'Alice',
    age: 30,
    address: { city: 'Wonderland' }
};
console.log(name, age, city);

const [first, , third, ...rest] = [10, 20, 30, 40, 50];
console.log(first, third, rest.join(', '));
