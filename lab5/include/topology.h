#pragma once

#include <iostream>
#include <list>

class Topology {
   public:
    struct TopoIter {
        std::list<std::list<int>>::iterator it1;
        std::list<int>::iterator it2;
    };
    void insert(int id, int parentId);
    TopoIter find(int id);
    void erase(int id);
    static Topology &get();

   private:
    Topology();
    Topology(Topology const &) = delete;
    Topology &operator=(Topology const &) = delete;
    using Topo = std::list<std::list<int>>;
    Topo lists;
    TopoIter end();
    TopoIter begin();
};

bool operator==(Topology::TopoIter it1, Topology::TopoIter it2) {
    return it1.it1 == it2.it1 && it1.it2 == it2.it2;
}