#include "router.h"

#include <gtest/gtest.h>
#include <stdexcept>

class RouterTest : public ::testing::Test {
    protected:
        using RouterNode = Router::RouterNode;

        Router router;

        const RouterNode& root() const { return router.root_; }

        static const RouterNode* child(const RouterNode& parent, const std::string& value) {
            const auto it = parent._static_children.find(value);
            return it == parent._static_children.end() ? nullptr : it->second.get();
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

    ASSERT_EQ(root()._static_children.size(), 1u);
    EXPECT_TRUE(root()._routes.empty());

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    EXPECT_TRUE(prefix->_routes.empty());
    ASSERT_EQ(prefix->_static_children.size(), 1u);

    const RouterNode* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    EXPECT_TRUE(users->_routes.empty());
    ASSERT_EQ(users->_static_children.size(), 1u);

    const RouterNode* add = child(*users, "add");

    ASSERT_NE(add, nullptr);
    expect_route(*add, HttpMethod::GET, "/users/add");
    EXPECT_TRUE(add->_static_children.empty());
}

TEST_F(RouterTest, ReusesSharedPrefixes) {
    router.add(HttpMethod::GET, "/users/add");
    router.add(HttpMethod::DELETE, "/users/delete");
    router.add(HttpMethod::POST, "/posts");

    ASSERT_EQ(root()._static_children.size(), 1u);

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->_static_children.size(), 2u);

    const RouterNode* users = child(*prefix, "users");
    const RouterNode* posts = child(*prefix, "posts");

    ASSERT_NE(users, nullptr);
    ASSERT_NE(posts, nullptr);
    EXPECT_TRUE(users->_routes.empty());
    expect_route(*posts, HttpMethod::POST, "/posts");
    EXPECT_TRUE(posts->_static_children.empty());
    ASSERT_EQ(users->_static_children.size(), 2u);

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

    ASSERT_EQ(root()._static_children.size(), 1u);

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->_static_children.size(), 1u);

    const RouterNode* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    expect_route(*users, HttpMethod::GET, "/users");
    EXPECT_TRUE(users->_static_children.empty());
}

TEST_F(RouterTest, RegisteringAPrefixPreservesItsChildren) {
    router.add(HttpMethod::GET, "/users/add");
    router.add(HttpMethod::GET, "/users");

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->_static_children.size(), 1u);

    const RouterNode* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    expect_route(*users, HttpMethod::GET, "/users");
    ASSERT_EQ(users->_static_children.size(), 1u);

    const RouterNode* add = child(*users, "add");
    
    ASSERT_NE(add, nullptr);
    expect_route(*add, HttpMethod::GET, "/users/add");
}

TEST_F(RouterTest, ExtendingARoutePreservesItsRouteMarker) {
    router.add(HttpMethod::GET, "/users");
    router.add(HttpMethod::GET, "/users/add");

    const RouterNode* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->_static_children.size(), 1u);

    const RouterNode* users = child(*prefix, "users");
    
    ASSERT_NE(users, nullptr);
    expect_route(*users, HttpMethod::GET, "/users");
    ASSERT_EQ(users->_static_children.size(), 1u);
    
    const RouterNode* add = child(*users, "add");
    
    ASSERT_NE(add, nullptr);
    expect_route(*add, HttpMethod::GET, "/users/add");
}

TEST_F(RouterTest, RejectsRoutesWithoutALeadingSlash) {
    EXPECT_THROW(router.add(HttpMethod::GET, "users/add"), std::invalid_argument);
    EXPECT_THROW(router.add(HttpMethod::GET, ""), std::invalid_argument);
    EXPECT_TRUE(root()._static_children.empty());
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
    EXPECT_TRUE(users->_static_children.empty());
    
    const RouterNode* id = users->_dynamic_child.get();
    
    ASSERT_NE(id, nullptr);
    ASSERT_EQ(id->_routes.size(), 2u);
    expect_route(*id, HttpMethod::GET, "/users/:id");
    expect_route(*id, HttpMethod::PUT, "/users/:id");
}

TEST_F(RouterTest, AddingAStaticRoutePreservesItsDynamicSibling) {
    router.add(HttpMethod::GET, "/users/:id");
    router.add(HttpMethod::GET, "/users/new");

    const RouterNode* prefix = slash();
    ASSERT_NE(prefix, nullptr);
    const RouterNode* users = child(*prefix, "users");
    ASSERT_NE(users, nullptr);

    const RouterNode* id = users->_dynamic_child.get();
    ASSERT_NE(id, nullptr);
    expect_route(*id, HttpMethod::GET, "/users/:id");

    ASSERT_EQ(users->_static_children.size(), 1u);
    const RouterNode* new_user = child(*users, "new");
    ASSERT_NE(new_user, nullptr);
    expect_route(*new_user, HttpMethod::GET, "/users/new");
    EXPECT_TRUE(users->_routes.empty());
}

TEST_F(RouterTest, AddingADynamicRoutePreservesItsStaticSibling) {
    router.add(HttpMethod::GET, "/users/new");
    router.add(HttpMethod::GET, "/users/:id");

    const RouterNode* prefix = slash();
    ASSERT_NE(prefix, nullptr);
    const RouterNode* users = child(*prefix, "users");
    ASSERT_NE(users, nullptr);

    const RouterNode* id = users->_dynamic_child.get();
    ASSERT_NE(id, nullptr);
    expect_route(*id, HttpMethod::GET, "/users/:id");

    ASSERT_EQ(users->_static_children.size(), 1u);
    const RouterNode* new_user = child(*users, "new");
    ASSERT_NE(new_user, nullptr);
    expect_route(*new_user, HttpMethod::GET, "/users/new");
    EXPECT_TRUE(users->_routes.empty());
}

TEST_F(RouterTest, MatchesDifferentValuesForAParameter) {
    router.add(HttpMethod::GET, "/users/:id");

    EXPECT_TRUE(router.contains(HttpMethod::GET, "/users/42"));
    EXPECT_TRUE(router.contains(HttpMethod::GET, "/users/alice"));
}

TEST_F(RouterTest, MatchesMultipleParametersWithStaticSegmentsBetweenThem) {
    router.add(HttpMethod::GET, "/users/:userId/posts/:postId");

    EXPECT_TRUE(router.contains(HttpMethod::GET, "/users/42/posts/7"));
    EXPECT_TRUE(router.contains(HttpMethod::GET, "/users/alice/posts/hello"));
    EXPECT_FALSE(router.contains(HttpMethod::GET, "/users/42/comments/7"));
}

TEST_F(RouterTest, PreservesRoutesSharingAParameterPrefix) {
    router.add(HttpMethod::GET, "/users/:id");
    router.add(HttpMethod::GET, "/users/:id/posts");
    router.add(HttpMethod::PUT, "/users/:id/settings");

    EXPECT_TRUE(router.contains(HttpMethod::GET, "/users/42"));
    EXPECT_TRUE(router.contains(HttpMethod::GET, "/users/42/posts"));
    EXPECT_TRUE(router.contains(HttpMethod::PUT, "/users/42/settings"));
    EXPECT_FALSE(router.contains(HttpMethod::GET, "/users/42/settings"));
    EXPECT_FALSE(router.contains(HttpMethod::GET, "/users/42/comments"));
}

TEST_F(RouterTest, ParameterMatchesRequireTheRegisteredMethodAndCompletePath) {
    router.add(HttpMethod::GET, "/users/:id/posts");

    EXPECT_TRUE(router.contains(HttpMethod::GET, "/users/42/posts"));
    EXPECT_FALSE(router.contains(HttpMethod::POST, "/users/42/posts"));
    EXPECT_FALSE(router.contains(HttpMethod::GET, "/users/posts"));
    EXPECT_FALSE(router.contains(HttpMethod::GET, "/users//posts"));
    EXPECT_FALSE(router.contains(HttpMethod::GET, "/users/42"));
    EXPECT_FALSE(router.contains(HttpMethod::GET, "/users/42/posts/7"));
}

TEST_F(RouterTest, MatchesLiteralEmptySegmentsAlongsideParameters) {
    router.add(HttpMethod::GET, "/users/:id/posts");
    router.add(HttpMethod::POST, "/users//posts");

    EXPECT_TRUE(router.contains(HttpMethod::GET, "/users/42/posts"));
    EXPECT_FALSE(router.contains(HttpMethod::GET, "/users//posts"));
    EXPECT_TRUE(router.contains(HttpMethod::POST, "/users//posts"));
    EXPECT_FALSE(router.contains(HttpMethod::POST, "/users/42/posts"));
    EXPECT_FALSE(router.contains(HttpMethod::POST, "/users/posts"));
}
