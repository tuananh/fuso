const t = require('tape')
const fs = require('fs')
const transform = require('../')

const json = fs.readFileSync('./store.json', 'utf-8')

t.test('basic test', t => {
    const output = transform(json, {
        authors: '$.store.book[?(@.price < 10)].author',
        first_book: {
            title: '$.store.book[0].title',
            price: '$.store.book[0].price',
            category: {
                in_here: '$.store.book[0].category'
            }
        },
        arr: [
            '$.store.book',
            {
                title: '$.title',
                category: '$.category'
            }
        ],
        string: '$.store.book[1].title'
    })
    t.equal(typeof output, 'object', 'output is expected to be an object')
    t.equal(
        output.string,
        'Sword of Honour',
        'output.string === Sword of Honour'
    )
    t.equal(
        output.first_book.title,
        'Sayings of the Century',
        'first_book.title === Sayings of the Century'
    )
    t.equal(output.first_book.price, '8.95', 'first_book.price === 8.95')
    t.equal(
        output.first_book.category.in_here,
        'reference',
        'first_book.category.in_here === reference'
    )

    t.end()
})

t.test('array test', t => {
    const { array } = transform(json, {
        array: [
            '$.store.book',
            {
                title: '$.title',
                category: '$.category'
            }
        ]
    })
    t.equal(Array.isArray(array), true, 'array is expected to be an array')

    t.end()
})

t.test('array test .. selector', t => {
    const { array } = transform(json, {
        array: [
            '$..book',
            {
                title: '$.title',
                category: '$.category'
            }
        ]
    })
    t.equal(Array.isArray(array), true, 'array is expected to be an array')

    t.end()
})
