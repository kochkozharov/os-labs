
#include <gtest/gtest.h>

#include "node.h"
#include "topology.h"

TEST(Lab5Tests, TopologyTest) {
    Topology &t = Topology::get();
    ASSERT_EQ(t.find(100), t.end());
    ASSERT_TRUE(t.insert(1, -1));
    ASSERT_EQ(t.find(1), t.begin());
    ASSERT_TRUE(t.insert(2, 1));
    auto it = t.begin();
    it.it2++;
    ASSERT_EQ(t.find(2), it);
    ASSERT_TRUE(t.insert(66, 2));
    it.it2++;
    ASSERT_EQ(t.find(66), it);
    ASSERT_TRUE(t.insert(3, -1));
    it.it1++;
    it.it2 = it.it1->begin();
    ASSERT_EQ(t.find(3), it);
    ASSERT_TRUE(t.insert(7, 3));
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

TEST(Lab5Tests, CalculationTest) {
    ASSERT_EQ(ComputationNode::findAllOccurencies("memmem", "mem"), "0;3");
    ASSERT_EQ(ComputationNode::findAllOccurencies("1000", "00"), "1;2");
}

TEST(Lab5Tests, ExecTest) {
    auto cstr = std::getenv("PATH_TO_SERVER");
    ASSERT_TRUE(cstr);
    pid_t pid = fork();
    if (pid == -1) {
        std::perror("fork");
        std::exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        execl(cstr, cstr, std::to_string(1).c_str(), std::to_string(-1).c_str(),
              nullptr);
    }
    ControlNode::get().send(1, "exec ooloo oo");
    ASSERT_EQ(ControlNode::get().receive().value(), "Ok: 1 0;3");
}