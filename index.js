const binary = require('node-pre-gyp')
const path = require('path')
const bindingPath = binary.find(
    path.resolve(path.join(__dirname, './package.json'))
)
const fuso = require(bindingPath)

function isNonEmptyString(str) {
    return typeof str === 'string' && str.length > 0
}

function transform(json, template) {
    if (!isNonEmptyString(json)) {
        throw new TypeError('1st argument (json) must be a non-empty string')
    }

    if (!template || typeof template !== 'object') {
        throw new TypeError('2nd argument (template) must be an object')
    }

    const template_string = JSON.stringify(template)
    let result = fuso.transform(json, template_string)

    if (!result) {
        throw new TypeError('Invalid input: Malformed json')
    }

    return result
}

module.exports = transform
