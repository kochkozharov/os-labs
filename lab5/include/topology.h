#pragma once

#include <iostream>
#include <list>

class Topology {
   private:
    std::list<std::list<int>> lists;

   public:
    Topology() = default;
    struct TopoIter {
        std::list<std::list<int>>::iterator it1;
        std::list<int>::iterator it2;
    };
    bool insert(int id, int parentId);
    TopoIter find(int id);
    bool erase(int id);
    TopoIter end();
    TopoIter begin();
    bool contains(int id);
};

bool operator==(Topology::TopoIter it1, Topology::TopoIter it2);

bool operator!=(Topology::TopoIter it1, Topology::TopoIter it2);