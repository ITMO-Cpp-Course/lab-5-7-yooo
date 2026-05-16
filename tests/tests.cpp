#define CATCH_CONFIG_MAIN

#include "Document.hpp"
#include "DocumentBuilder.hpp"
#include "IndexStore.hpp"
#include "InvertedIndex.hpp"
#include "UpdateTransaction.hpp"
#include <catch2/catch_all.hpp>

using namespace lab5;

TEST_CASE("DocumentBuilder works correctly", "[builder]")
{
    DocumentBuilder builder;

    SECTION("Parses standard text and removes punctuation")
    {
        auto parsed = builder.Build(1, "test.txt", "Hello, World! Hello.");

        REQUIRE(parsed.doc.GetId() == 1);
        REQUIRE(parsed.doc.GetName() == "test.txt");

        REQUIRE(parsed.words.size() == 3);
        CHECK(parsed.words[0] == "hello");
        CHECK(parsed.words[1] == "world");
        CHECK(parsed.words[2] == "hello");
    }

    SECTION("Handles text without special characters")
    {
        auto parsed = builder.Build(2, "dev.txt", "I love C plus plus");

        REQUIRE(parsed.words.size() == 5);
        CHECK(parsed.words[0] == "i");
        CHECK(parsed.words[1] == "love");
        CHECK(parsed.words[2] == "c");
        CHECK(parsed.words[3] == "plus");
        CHECK(parsed.words[4] == "plus");
    }
}

TEST_CASE("InvertedIndex operations", "[index]")
{
    DocumentBuilder builder;
    InvertedIndex index;

    index.AddDocument(builder.Build(1, "doc1.txt", "Hello world! Hello."));
    index.AddDocument(builder.Build(2, "doc2.txt", "The world is big. Hello again."));

    SECTION("Searching for existing words")
    {
        auto results_hello = index.Search("hello");
        REQUIRE(results_hello.size() == 2);
        CHECK(results_hello[1] == 2);
        CHECK(results_hello[2] == 1);

        auto results_world = index.Search("world");
        REQUIRE(results_world.size() == 2);
        CHECK(results_world[1] == 1);
        CHECK(results_world[2] == 1);
    }

    SECTION("Searching for missing words returns empty map")
    {
        auto results_none = index.Search("missing");
        REQUIRE(results_none.empty());
    }

    SECTION("Removing an existing document updates index correctly")
    {
        index.RemoveDocument(1);

        auto results_hello = index.Search("hello");
        REQUIRE(results_hello.size() == 1);
        CHECK(results_hello.count(1) == 0);
        CHECK(results_hello[2] == 1);

        CHECK(!index.GetDocument(1).has_value());
        REQUIRE(index.GetDocument(2).has_value());
    }

    SECTION("Removing a non-existent document does not break state")
    {
        REQUIRE_NOTHROW(index.RemoveDocument(999));

        auto results_hello = index.Search("hello");
        REQUIRE(results_hello.size() == 2);
    }

    SECTION("Handles various punctuation marks as separators")
    {
        auto parsed = builder.Build(3, "punct.txt", "one?two;three:four-five(six)seven/eight");

        REQUIRE(parsed.words.size() == 8);
        CHECK(parsed.words[0] == "one");
        CHECK(parsed.words[1] == "two");
        CHECK(parsed.words[2] == "three");
        CHECK(parsed.words[3] == "four");
        CHECK(parsed.words[4] == "five");
        CHECK(parsed.words[5] == "six");
        CHECK(parsed.words[6] == "seven");
        CHECK(parsed.words[7] == "eight");
    }
}

// 6ТЕСТЫ

TEST_CASE("IndexStore and UpdateTransaction", "[store]")
{
    DocumentBuilder builder;
    IndexStore store;

    SECTION("AddDocument directly to store handles Expected")
    {
        auto res = store.AddDocument(builder.Build(1, "doc1.txt", "hello world"));
        REQUIRE(res.has_value());

        auto search_res = store.Search("hello");
        REQUIRE(search_res.has_value());
        REQUIRE(search_res->size() == 1);

        // Попытка добавить документ с таким же ID должна выдать ошибку
        auto res2 = store.AddDocument(builder.Build(1, "doc1.txt", "again"));
        REQUIRE_FALSE(res2.has_value());
        REQUIRE(res2.error().code == ErrorCode::DocumentAlreadyExists);
    }

    SECTION("RemoveDocument directly from store with error checks")
    {
        REQUIRE(store.AddDocument(builder.Build(1, "doc1.txt", "hello world")).has_value());

        auto res = store.RemoveDocument(1);
        REQUIRE(res.has_value());

        auto search_res = store.Search("hello");
        REQUIRE(search_res.has_value());
        REQUIRE(search_res->empty());

        // Попытка удалить несуществующий документ
        auto res2 = store.RemoveDocument(1);
        REQUIRE_FALSE(res2.has_value());
        REQUIRE(res2.error().code == ErrorCode::DocumentNotFound);
    }

    SECTION("Transactions - successful commit")
    {
        auto tx_res = store.BeginTransaction();
        REQUIRE(tx_res.has_value());
        auto& tx = tx_res.value();

        auto add_res1 = tx.AddDocument(builder.Build(1, "doc1.txt", "transaction test"));
        REQUIRE(add_res1.has_value());

        // До коммита изменения не должны быть видны
        auto search_res = store.Search("transaction");
        REQUIRE(search_res.has_value());
        REQUIRE(search_res->empty());

        auto commit_res = tx.Commit();
        REQUIRE(commit_res.has_value());

        // После коммита должны появиться
        search_res = store.Search("transaction");
        REQUIRE(search_res.has_value());
        REQUIRE(search_res->size() == 1);
    }

    SECTION("Transactions - rollback on automatic destruction")
    {
        {
            auto tx_res = store.BeginTransaction();
            REQUIRE(tx_res.has_value());
            auto& tx = tx_res.value();

            // ИСПРАВЛЕНИЕ ТУТ: проверяем результат через REQUIRE
            REQUIRE(tx.AddDocument(builder.Build(1, "doc1.txt", "rollback test")).has_value());
            // Выход из области видимости, commit() не вызывается
        }

        auto search_res = store.Search("rollback");
        REQUIRE(search_res.has_value());
        REQUIRE(search_res->empty());
    }

    SECTION("Transactions - complex operations correctly applied")
    {
        //проверяем результат через REQUIRE
        REQUIRE(store.AddDocument(builder.Build(1, "doc1.txt", "keep me")).has_value());
        REQUIRE(store.AddDocument(builder.Build(2, "doc2.txt", "remove me")).has_value());

        auto tx_res = store.BeginTransaction();
        auto& tx = tx_res.value();

        REQUIRE(tx.RemoveDocument(2).has_value());
        REQUIRE(tx.AddDocument(builder.Build(3, "doc3.txt", "new doc")).has_value());

        REQUIRE_FALSE(tx.RemoveDocument(4).has_value());

        REQUIRE(tx.Commit().has_value());

        REQUIRE(store.Search("keep")->size() == 1);
        REQUIRE(store.Search("remove")->empty());
        REQUIRE(store.Search("new")->size() == 1);

        REQUIRE(store.GetDocument(1).has_value());
        REQUIRE_FALSE(store.GetDocument(2).has_value());
        REQUIRE(store.GetDocument(3).has_value());
    }
}