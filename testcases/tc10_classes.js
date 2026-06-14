class Animal {
    constructor(name, sound) {
        this.name = name;
        this.sound = sound;
    }
    speak() {
        return `${this.name} says ${this.sound}`;
    }
}
class Dog extends Animal {
    constructor(name) {
        super(name, 'Woof');
    }
    fetch(item) {
        return `${this.name} fetches the ${item}!`;
    }
}
const d = new Dog('Rex');
console.log(d.speak());
console.log(d.fetch('ball'));
console.log(d instanceof Animal); // true
