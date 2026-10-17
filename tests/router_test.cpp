#include "router.h"

#include <gtest/gtest.h>
#include <stdexcept>

class RouterTest : public ::testing::Test {
    protected:
        using RouterNode = Router::RouterNode;

        Router router;

        const RouterNode& root() const { return router._root; }

        static const RouterNode* child(const RouterNode& parent, const std::string& value) {
            const auto it = parent._children.find(value);
            return it == parent._children.end() ? nullptr : it->second.get();
        }

        static void expect_route(const RouterNode& node, HttpMethod method, const std::string& path) {
            const auto it = node._routes.find(method);
            ASSERT_NE(it, node._routes.end());
            EXPECT_EQ(it->second, path);
        }

        // The current router stores the leading slash as an empty segment.
        const RouterNode* slash() const { return child(root(), ""); }
};

TEST_F(RouterTest, CreatesSegmentsAndMarksOnlyTheFinalNodeAsARoute) {
    router.add(HttpMethod::GET, "/users/add");

    ASSERT_EQ(root()._children.size(), 1u);
    EXPECT_TRUE(root()._routes.empty());

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    EXPECT_TRUE(prefix->_routes.empty());
    ASSERT_EQ(prefix->_children.size(), 1u);

    const RouterNode* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    EXPECT_TRUE(users->_routes.empty());
    ASSERT_EQ(users->_children.size(), 1u);

    const RouterNode* add = child(*users, "add");

    ASSERT_NE(add, nullptr);
    expect_route(*add, HttpMethod::GET, "/users/add");
    EXPECT_TRUE(add->_children.empty());
}

TEST_F(RouterTest, ReusesSharedPrefixes) {
    router.add(HttpMethod::GET, "/users/add");
    router.add(HttpMethod::DELETE, "/users/delete");
    router.add(HttpMethod::POST, "/posts");

    ASSERT_EQ(root()._children.size(), 1u);

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->_children.size(), 2u);

    const RouterNode* users = child(*prefix, "users");
    const RouterNode* posts = child(*prefix, "posts");

    ASSERT_NE(users, nullptr);
    ASSERT_NE(posts, nullptr);
    EXPECT_TRUE(users->_routes.empty());
    expect_route(*posts, HttpMethod::POST, "/posts");
    EXPECT_TRUE(posts->_children.empty());
    ASSERT_EQ(users->_children.size(), 2u);

    const RouterNode* add = child(*users, "add");
    const RouterNode* remove = child(*users, "delete");

    ASSERT_NE(add, nullptr);
    ASSERT_NE(remove, nullptr);
    expect_route(*add, HttpMethod::GET, "/users/add");
    expect_route(*remove, HttpMethod::DELETE, "/users/delete");
}

TEST_F(RouterTest, AddingADuplicateDoesNotCreateMoreNodes) {
    router.add(HttpMethod::GET, "/users");
    router.add(HttpMethod::GET, "/users");

    ASSERT_EQ(root()._children.size(), 1u);

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->_children.size(), 1u);

    const RouterNode* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    expect_route(*users, HttpMethod::GET, "/users");
    EXPECT_TRUE(users->_children.empty());
}

TEST_F(RouterTest, RegisteringAPrefixPreservesItsChildren) {
    router.add(HttpMethod::GET, "/users/add");
    router.add(HttpMethod::GET, "/users");

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->_children.size(), 1u);

    const RouterNode* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    expect_route(*users, HttpMethod::GET, "/users");
    ASSERT_EQ(users->_children.size(), 1u);

    const RouterNode* add = child(*users, "add");
    
    ASSERT_NE(add, nullptr);
    expect_route(*add, HttpMethod::GET, "/users/add");
}

TEST_F(RouterTest, ExtendingARoutePreservesItsRouteMarker) {
    router.add(HttpMethod::GET, "/users");
    router.add(HttpMethod::GET, "/users/add");

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->_children.size(), 1u);

    const RouterNode* users = child(*prefix, "users");
    
    ASSERT_NE(users, nullptr);
    expect_route(*users, HttpMethod::GET, "/users");
    ASSERT_EQ(users->_children.size(), 1u);
    
    const RouterNode* add = child(*users, "add");
    
    ASSERT_NE(add, nullptr);
    expect_route(*add, HttpMethod::GET, "/users/add");
}

TEST_F(RouterTest, RejectsRoutesWithoutALeadingSlash) {
    EXPECT_THROW(router.add(HttpMethod::GET, "users/add"), std::invalid_argument);
    EXPECT_THROW(router.add(HttpMethod::GET, ""), std::invalid_argument);
    EXPECT_TRUE(root()._children.empty());
    EXPECT_TRUE(root()._routes.empty());
}

TEST_F(RouterTest, PreservesDifferentMethodsForTheSamePath) {
    router.add(HttpMethod::GET, "/users/:id");
    router.add(HttpMethod::PUT, "/users/:id");
    router.add(HttpMethod::GET, "/users/:id");

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);

    const RouterNode* users = child(*prefix, "users");
    
    ASSERT_NE(users, nullptr);
    ASSERT_EQ(users->_children.size(), 1u);
    
    const RouterNode* id = child(*users, ":id");
    
    ASSERT_NE(id, nullptr);
    ASSERT_EQ(id->_routes.size(), 2u);
    expect_route(*id, HttpMethod::GET, "/users/:id");
    expect_route(*id, HttpMethod::PUT, "/users/:id");
}

TEST_F(RouterTest, PrintsMethodsOnlyOnRegisteredRoutes) {
    router.add(HttpMethod::GET, "/users/:id");
    router.add(HttpMethod::PUT, "/users/:id");
    router.add(HttpMethod::POST, "/users/:id");
    router.add(HttpMethod::DELETE, "/users/:id");
    router.add(HttpMethod::GET, "/users");

    testing::internal::CaptureStdout();
    router.print();
    const std::string output = testing::internal::GetCapturedStdout();

    EXPECT_EQ(output,
              "└── \n"
              "    └── users [GET]\n"
              "        └── :id [GET, POST, DELETE, PUT]\n");
}

TEST_F(RouterTest, PrintingAnEmptyRouterProducesNoOutput) {
    testing::internal::CaptureStdout();
    router.print();
    EXPECT_TRUE(testing::internal::GetCapturedStdout().empty());
}
