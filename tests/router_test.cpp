#include "router.h"

#include <gtest/gtest.h>
#include <stdexcept>

class RouterAddTest : public ::testing::Test {
    protected:
        Router router;

        const Node& root() const { return router._root; }

        static const Node* child(const Node& parent, const std::string& value) {
            for (const auto& node : parent.children) {
                if (node->value == value) {
                    return node.get();
                }
            }
            return nullptr;
        }

        // The current router stores the leading slash as an empty segment.
        const Node* slash() const { return child(root(), ""); }
};

TEST_F(RouterAddTest, CreatesSegmentsAndMarksOnlyTheFinalNodeAsARoute) {
    router.add("/users/add");

    ASSERT_EQ(root().children.size(), 1u);
    EXPECT_FALSE(root().is_route);

    const Node* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    EXPECT_FALSE(prefix->is_route);
    ASSERT_EQ(prefix->children.size(), 1u);

    const Node* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    EXPECT_FALSE(users->is_route);
    ASSERT_EQ(users->children.size(), 1u);

    const Node* add = child(*users, "add");

    ASSERT_NE(add, nullptr);
    EXPECT_TRUE(add->is_route);
    EXPECT_TRUE(add->children.empty());
}

TEST_F(RouterAddTest, ReusesSharedPrefixes) {
    router.add("/users/add");
    router.add("/users/delete");
    router.add("/posts");

    ASSERT_EQ(root().children.size(), 1u);

    const Node* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->children.size(), 2u);

    const Node* users = child(*prefix, "users");
    const Node* posts = child(*prefix, "posts");

    ASSERT_NE(users, nullptr);
    ASSERT_NE(posts, nullptr);
    EXPECT_FALSE(users->is_route);
    EXPECT_TRUE(posts->is_route);
    EXPECT_TRUE(posts->children.empty());
    ASSERT_EQ(users->children.size(), 2u);

    const Node* add = child(*users, "add");
    const Node* remove = child(*users, "delete");

    ASSERT_NE(add, nullptr);
    ASSERT_NE(remove, nullptr);
    EXPECT_TRUE(add->is_route);
    EXPECT_TRUE(remove->is_route);
}

TEST_F(RouterAddTest, AddingADuplicateDoesNotCreateMoreNodes) {
    router.add("/users");
    router.add("/users");

    ASSERT_EQ(root().children.size(), 1u);

    const Node* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->children.size(), 1u);

    const Node* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    EXPECT_TRUE(users->is_route);
    EXPECT_TRUE(users->children.empty());
}

TEST_F(RouterAddTest, RegisteringAPrefixPreservesItsChildren) {
    router.add("/users/add");
    router.add("/users");

    const Node* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->children.size(), 1u);

    const Node* users = child(*prefix, "users");

    ASSERT_NE(users, nullptr);
    EXPECT_TRUE(users->is_route);
    ASSERT_EQ(users->children.size(), 1u);

    const Node* add = child(*users, "add");
    
    ASSERT_NE(add, nullptr);
    EXPECT_TRUE(add->is_route);
}

TEST_F(RouterAddTest, ExtendingARoutePreservesItsRouteMarker) {
    router.add("/users");
    router.add("/users/add");

    const Node* prefix = slash();

    ASSERT_NE(prefix, nullptr);
    ASSERT_EQ(prefix->children.size(), 1u);

    const Node* users = child(*prefix, "users");
    
    ASSERT_NE(users, nullptr);
    EXPECT_TRUE(users->is_route);
    ASSERT_EQ(users->children.size(), 1u);
    
    const Node* add = child(*users, "add");
    
    ASSERT_NE(add, nullptr);
    EXPECT_TRUE(add->is_route);
}

TEST_F(RouterAddTest, RejectsRoutesWithoutALeadingSlash) {
    EXPECT_THROW(router.add("users/add"), std::invalid_argument);
    EXPECT_THROW(router.add(""), std::invalid_argument);
    EXPECT_TRUE(root().children.empty());
    EXPECT_FALSE(root().is_route);
}
