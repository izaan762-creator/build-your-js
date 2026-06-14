function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}
async function main() {
    await delay(0);
    console.log("Promise resolved");
    const val = await Promise.resolve(42);
    console.log("Value:", val);
}
main();
