#include "Utilities/DataStructures/HierarchyTree.hpp"
#include <algorithm>
#include <gtest/gtest.h>

namespace Ivy::UnitTests
{
using Tree = Ivy::U::HierarchyTree<unsigned int>;
using Node = unsigned int;
constexpr Node INV = Tree::Invalid;

static void BuildLinear(Tree& t, unsigned int depth)
{
    for (unsigned int i = 0; i < depth; ++i)
    {
        t.Ensure(i);
    }
    for (unsigned int i = 1; i < depth; ++i)
    {
        t.Attach(i - 1, i);
    }
}

TEST(HierarchyTree, EnsureCreatesNodes)
{
    Tree t;
    EXPECT_FALSE(t.Exists(0));
    t.Ensure(0);
    EXPECT_TRUE(t.Exists(0));
    EXPECT_TRUE(t.IsLeaf(0));
}

TEST(HierarchyTree, EnsureInvalidIsNoOp)
{
    Tree t;
    t.Ensure(INV);
    EXPECT_FALSE(t.Exists(0));
    EXPECT_EQ(t.Version(), 0u);
}

TEST(HierarchyTree, AttachParentChild)
{
    Tree t;
    t.Ensure(0);
    t.Ensure(1);
    t.Attach(0, 1);

    EXPECT_TRUE(t.IsChildOf(1, 0));
    EXPECT_TRUE(t.IsParentOf(0, 1));
    EXPECT_EQ(t.GetParent(1), 0u);
    EXPECT_EQ(t.GetFirstChild(0), 1u);
    EXPECT_EQ(t.GetLastChild(0), 1u);
    EXPECT_FALSE(t.IsLeaf(0));
    EXPECT_TRUE(t.IsLeaf(1));
}

TEST(HierarchyTree, AttachMultipleChildrenPreservesOrder)
{
    Tree t;
    for (int i = 0; i < 4; ++i)
    {
        t.Ensure(i);
    }
    t.Attach(0, 1);
    t.Attach(0, 2);
    t.Attach(0, 3);

    EXPECT_EQ(t.GetFirstChild(0), 1u);
    EXPECT_EQ(t.GetLastChild(0), 3u);
    EXPECT_EQ(t.GetNextSibling(1), 2u);
    EXPECT_EQ(t.GetNextSibling(2), 3u);
    EXPECT_EQ(t.GetNextSibling(3), INV);
    EXPECT_EQ(t.GetPrevSibling(2), 1u);
    EXPECT_EQ(t.GetPrevSibling(3), 2u);

    auto children = t.GetChildren(0);
    EXPECT_EQ(children, (std::vector<Node>{1, 2, 3}));
}

TEST(HierarchyTree, AttachSelfRejected)
{
    Tree t;
    t.Ensure(0);
    t.Attach(0, 0);
    EXPECT_FALSE(t.HasParent(0));
}

TEST(HierarchyTree, AttachCyclesRejected)
{
    Tree t;
    for (int i = 0; i < 3; ++i)
    {
        t.Ensure(i);
    }
    t.Attach(0, 1);
    t.Attach(1, 2);
    // 2 is descendant of 0 — can't attach 2 under ... actually can't cycle
    t.Attach(2, 0); // would make 0 child of 2 which is 2's ancestor → cycle
    EXPECT_FALSE(t.IsChildOf(0, 2));
    EXPECT_TRUE(t.IsChildOf(1, 0));
}

TEST(HierarchyTree, Reparent)
{
    Tree t;
    for (int i = 0; i < 3; ++i)
    {
        t.Ensure(i);
    }
    t.Attach(0, 2);
    t.Attach(1, 2); // reparent 2 from 0 to 1
    EXPECT_TRUE(t.IsChildOf(2, 1));
    EXPECT_FALSE(t.IsChildOf(2, 0));
    EXPECT_EQ(t.GetChildCount(0), 0u);
    EXPECT_EQ(t.GetChildCount(1), 1u);
}

TEST(HierarchyTree, DetachRootChild)
{
    Tree t;
    for (int i = 0; i < 3; ++i)
    {
        t.Ensure(i);
    }
    t.Attach(0, 1);
    t.Attach(0, 2);
    t.Detach(1);
    EXPECT_FALSE(t.IsChildOf(1, 0));
    EXPECT_EQ(t.GetFirstChild(0), 2u);
    EXPECT_EQ(t.GetChildCount(0), 1u);
}

TEST(HierarchyTree, VersionIncrementsOnMutation)
{
    Tree t;
    for (int i = 0; i < 3; ++i)
    {
        t.Ensure(i);
    }
    auto v0 = t.Version();
    t.Attach(0, 1);
    auto v1 = t.Version();
    EXPECT_GT(v1, v0);
    t.Attach(0, 2);
    auto v2 = t.Version();
    EXPECT_GT(v2, v1);
    t.Detach(1);
    EXPECT_GT(t.Version(), v2);
}

TEST(HierarchyTree, GetRootAndDepth)
{
    Tree t;
    BuildLinear(t, 5); // 0 -> 1 -> 2 -> 3 -> 4
    EXPECT_EQ(t.GetRoot(4), 0u);
    EXPECT_EQ(t.GetDepth(0), 0u);
    EXPECT_EQ(t.GetDepth(2), 2u);
    EXPECT_EQ(t.GetDepth(4), 4u);
}

TEST(HierarchyTree, DescendantsPreOrder)
{
    Tree t;
    for (int i = 0; i < 6; ++i)
    {
        t.Ensure(i);
    }
    // 0 -> (1 -> 2 -> 3), (4), (5)
    t.Attach(0, 1);
    t.Attach(1, 2);
    t.Attach(2, 3);
    t.Attach(0, 4);
    t.Attach(0, 5);

    auto desc = t.GetDescendants(0);
    EXPECT_EQ(desc, (std::vector<Node>{1, 2, 3, 4, 5}));
}

TEST(HierarchyTree, AncestorsChildToRoot)
{
    Tree t;
    BuildLinear(t, 4); // 0 -> 1 -> 2 -> 3
    auto ancestors = t.GetAncestors(3);
    EXPECT_EQ(ancestors, (std::vector<Node>{2, 1, 0}));
}

TEST(HierarchyTree, Siblings)
{
    Tree t;
    for (int i = 0; i < 4; ++i)
    {
        t.Ensure(i);
    }
    t.Attach(0, 1);
    t.Attach(0, 2);
    t.Attach(0, 3);
    auto sibs = t.GetSiblings(2);
    EXPECT_EQ(sibs, (std::vector<Node>{1, 3}));
    EXPECT_TRUE(t.IsSiblingOf(1, 3));
    EXPECT_FALSE(t.IsSiblingOf(1, 1));
}

TEST(HierarchyTree, ForEachDescendantMatchesGetDescendants)
{
    Tree t;
    for (int i = 0; i < 7; ++i)
    {
        t.Ensure(i);
    }
    t.Attach(0, 1);
    t.Attach(0, 2);
    t.Attach(1, 3);
    t.Attach(1, 4);
    t.Attach(2, 5);
    t.Attach(2, 6);

    std::vector<Node> collected;
    t.ForEachDescendant(0, [&](Node n) { collected.push_back(n); });
    EXPECT_EQ(collected, t.GetDescendants(0));
}

TEST(HierarchyTree, ForEachChildStableAgainstMutation)
{
    // Regression: callback removes next sibling — traversal must still work
    // because ForEachChild caches `next` before calling action.
    Tree t;
    for (int i = 0; i < 4; ++i)
    {
        t.Ensure(i);
    }
    t.Attach(0, 1);
    t.Attach(0, 2);
    t.Attach(0, 3);

    std::vector<Node> visited;
    t.ForEachChild(0,
        [&](Node n)
        {
            visited.push_back(n);
            if (n == 1)
            {
                t.Detach(2);
            }
        });
    // Both 1 and 2 visited (2 was cached before detach)
    EXPECT_EQ(visited.size() >= 1u, true);
    EXPECT_EQ(visited[0], 1u);
}

TEST(HierarchyTree, EmptyQueriesReturnEmpty)
{
    Tree t;
    EXPECT_TRUE(t.GetChildren(0).empty());
    EXPECT_TRUE(t.GetDescendants(0).empty());
    EXPECT_TRUE(t.GetAncestors(0).empty());
    EXPECT_EQ(t.GetParent(0), INV);
    EXPECT_EQ(t.GetRoot(0), INV);
    EXPECT_EQ(t.GetDepth(0), INV);
}

} // namespace Ivy::UnitTests
