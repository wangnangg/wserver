#pragma once
#include <string>
#include <vector>

class Path
{
public:
    class Node
    {
        std::string _name;

    public:
        Node(std::string name) : _name(std::move(name)) {}
        const std::string& name() const { return _name; }
    };
    Path(const std::string& path) { parsePath(path); }
    const Node* begin() const { return &*_nodes.begin(); }
    const Node* end() const { return &*_nodes.end(); }
    int size() const { return _nodes.size(); }

private:
    std::vector<Node> _nodes;

    void parsePath(const std::string& path);
};
