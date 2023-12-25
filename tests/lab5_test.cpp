
#include <gtest/gtest.h>
#include "node.h"
#include "topology.h"


TEST(Lab5Tests, TopologyTest) {
    Topology &t = Topology::get();
    ASSERT_EQ(t.find(100), t.end());
    ASSERT_TRUE(t.insert(1, -1));
    ASSERT_EQ(t.find(1), t.begin());
    ASSERT_TRUE(t.insert(2,1));
    auto it = t.begin();
    it.it2++;
    ASSERT_EQ(t.find(2), it);
    ASSERT_TRUE(t.insert(66,2));
    it.it2++;
    ASSERT_EQ(t.find(66), it);
    ASSERT_TRUE(t.insert(3,-1));
    it.it1++;
    it.it2 = it.it1->begin();
    ASSERT_EQ(t.find(3), it);
    ASSERT_TRUE(t.insert(7,3));
    it.it2++;
    ASSERT_EQ(t.find(7), it);
    ASSERT_FALSE(t.insert(2, -1));
    ASSERT_FALSE(t.insert(6, -4));
    ASSERT_EQ(t.find(100), t.end());
    ASSERT_TRUE(t.erase(1));
    ASSERT_EQ(t.find(1), t.end());
    ASSERT_EQ(t.find(2), t.begin());
    ASSERT_TRUE(t.erase(2));
    ASSERT_TRUE(t.erase(66));
    ASSERT_EQ(t.find(3), t.begin());
    ASSERT_TRUE(t.insert(2, 3));
    it = t.begin();
    it.it2++;
    ASSERT_EQ(t.find(2), it);
}

