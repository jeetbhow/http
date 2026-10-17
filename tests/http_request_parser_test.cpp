#include "http.h"

#include <gtest/gtest.h>

TEST(HttpRequestParser, ParsesRequestLine) {
    HttpRequestParser parser{
        "GET /example HTTP/1.1\r\n"
        "\r\n"
    };

    const auto result = parser.parse();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->method, HttpMethod::GET);
    EXPECT_EQ(result->target, "/example");
    EXPECT_EQ(result->version, "1.1");
}

TEST(HttpRequestParser, ParsesHeaders) {
    HttpRequestParser parser{
        "GET /example HTTP/1.1\r\n"
        "Host: example-host\r\n"
        "User-Agent: http-test-client\r\n"
        "Connection: close\r\n"
        "\r\n"
    };

    const auto result = parser.parse();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->get_header("Host"), "example-host");
    EXPECT_EQ(result->get_header("host"), "example-host");
    EXPECT_EQ(result->get_header("User-Agent"), "http-test-client");
    EXPECT_EQ(result->get_header("CONNECTION"), "close");
    EXPECT_EQ(result->get_header("Missing"), std::nullopt);
}

TEST(HttpRequest, MissingHeaderReturnsNullopt) {
    const HttpRequest request{HttpMethod::GET, "/", "1.1"};
    EXPECT_EQ(request.get_header("Host"), std::nullopt);
}
