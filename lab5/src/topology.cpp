#include "topology.h"

Topology::NodeId::operator int() {
    return id;
}

Topology::TopoIter Topology::begin() {
    return {lists.begin(), lists.begin()->begin()};
}
Topology::TopoIter Topology::end() { return {lists.end(), lists.end()->end()}; }

bool Topology::insert(NodeId id, NodeId parentId) {
    auto coord = find(id);
    if (coord != end() || id == -1) {
        std::cerr << "Error: Already exists\n";
        return false;
    }
    if (parentId == -1) {
        lists.insert(lists.end(), std::list(1, id));
        return true;
    }
    auto coordParent = find(parentId);
    if (coordParent == end()) {
        std::cerr << "Error: Parent not found\n";
        return false;
    }
    if (coordParent.it2 != (coordParent.it1)->end()) {
        coordParent.it2++;
    }
    coordParent.it1->insert(coordParent.it2, id);
    return true;
}

Topology::TopoIter Topology::find(NodeId id) {
    auto it = begin();
    for (size_t i = 0; i < lists.size(); ++i) {
        it.it2 = it.it1->begin();
        for (size_t j = 0; j < (*it.it1).size(); ++j) {
            if (*it.it2 == id) {
                return it;
            }
            it.it2++;
        }
        it.it1++;
    }
    return end();
}

bool Topology::erase(NodeId id) {
    auto coord = find(id);
    if (coord == end() || id == -1) {
        return false;
    }
    coord.it1->erase(coord.it2);
    if (coord.it1->size() == 0) {
        lists.erase(coord.it1);
    }
    return true;
}

bool operator==(Topology::TopoIter it1, Topology::TopoIter it2) {
    return it1.it1 == it2.it1 && it1.it2 == it2.it2;
}

bool operator!=(Topology::TopoIter it1, Topology::TopoIter it2) {
    return !(it1 == it2);
}

bool Topology::contains(NodeId id) { return find(id) != end(); }
