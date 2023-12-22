#include "topology.h"

Topology::TopoIter Topology::begin() {
    return {lists.begin(), lists.begin()->begin()};
}
Topology::TopoIter Topology::end() { return {lists.end(), lists.end()->end()}; }

void Topology::insert(int id, int parentId) {
    auto coord = find(id);
    if (coord != end()) {
        throw std::runtime_error("Error: Already exists");
    }
    if (parentId == -1) {
        lists.insert(lists.end(), std::list(1, id));
        return;
    }
    auto coordParent = find(parentId);
    if (coordParent == end()) {
        throw std::runtime_error("Error: Parent not found");
    }
    coordParent.it1->insert(coordParent.it2++, id);
}

Topology::TopoIter Topology::find(int id) {
    auto it = begin();
    for (auto &lst : lists) {
        for (auto n : lst) {
            if (n == id) {
                return it;
            }
            it.it2++;
        }
        it.it1++;
    }
    return it;
}

void Topology::erase(int id) {
    auto coord = find(id);
    if (coord != end()) {
        throw std::runtime_error("Error: Not exists");
    }
    coord.it1->erase(coord.it2);
}

Topology &Topology::get() {
    static Topology instance;
    return instance;
}