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

    auto result = parser.parse();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->headers["Host"], "example-host");
    EXPECT_EQ(result->headers["User-Agent"], "http-test-client");
    EXPECT_EQ(result->headers["Connection"], "close");
}
