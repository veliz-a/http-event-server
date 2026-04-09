#include "http_parser.hpp"
#include <gtest/gtest.h>

TEST(HttpParserTest, ParsesGetRequest) {
    HttpParser parser;
    HttpRequest req;

    std::string raw =
        "GET /events HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Accept: application/json\r\n"
        "\r\n";

    EXPECT_TRUE(parser.parse(raw, req));
    EXPECT_EQ(req.method, "GET");
    EXPECT_EQ(req.path, "/events");
    EXPECT_EQ(req.version, "HTTP/1.1");
    EXPECT_EQ(req.headers["Host"], "localhost:8080");
    EXPECT_EQ(req.headers["Accept"], "application/json");
}

TEST(HttpParserTest, ParsesPostWithBody) {
    HttpParser parser;
    HttpRequest req;

    std::string raw =
        "POST /events HTTP/1.1\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 28\r\n"
        "\r\n"
        "{\"type\":\"login\",\"user\":\"42\"}";

    EXPECT_TRUE(parser.parse(raw, req));
    EXPECT_EQ(req.method, "POST");
    EXPECT_EQ(req.body, "{\"type\":\"login\",\"user\":\"42\"}");
}

TEST(HttpParserTest, ReturnsFalseOnMalformedRequest) {
    HttpParser parser;
    HttpRequest req;

    std::string raw = "esto no es http para nada";

    EXPECT_FALSE(parser.parse(raw, req));
}

TEST(HttpParserTest, SerializesResponseCorrectly) {
    HttpResponse res = make_response(200, "{\"status\":\"ok\"}");

    std::string serialized = res.serialize();

    EXPECT_NE(serialized.find("HTTP/1.1 200 OK"), std::string::npos);
    EXPECT_NE(serialized.find("Content-Length: 15"), std::string::npos);
    EXPECT_NE(serialized.find("{\"status\":\"ok\"}"), std::string::npos);
}