#define CATCH_CONFIG_MAIN

#include "Document.h"
#include "DocumentBuilder.h"
#include "InvertedIndex.h"
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

        CHECK(index.GetDocument(1) == nullptr);
        REQUIRE(index.GetDocument(2) != nullptr);
    }

    SECTION("Removing a non-existent document does not break state")
    {
        REQUIRE_NOTHROW(index.RemoveDocument(999));

        auto results_hello = index.Search("hello");
        REQUIRE(results_hello.size() == 2);
    }
}