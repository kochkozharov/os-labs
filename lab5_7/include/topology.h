#pragma once

#include <iostream>
#include <list>

class Topology {
   public:
    Topology() = default;
    struct NodeId {
        NodeId(int id) : id(id), pid(-1) {}
        NodeId(int id, int pid) : id(id), pid(pid) {}
        int id;
        int pid;
        operator int();
    };
    struct TopoIter {
        std::list<std::list<NodeId>>::iterator it1;
        std::list<NodeId>::iterator it2;
    };
    bool insert(NodeId id, NodeId parentId);
    TopoIter find(NodeId id);
    bool erase(NodeId id);
    TopoIter end();
    TopoIter begin();
    bool contains(NodeId id);

   private:
    std::list<std::list<NodeId>> lists;
};

bool operator==(Topology::TopoIter it1, Topology::TopoIter it2);

bool operator!=(Topology::TopoIter it1, Topology::TopoIter it2);