#ifndef MARKER_INDEX_H_
#define MARKER_INDEX_H_

#include <map>
#include <random>
#include <set>
#include <unordered_set>

#include "iterator.h"
#include "marker-id.h"
#include "node.h"

struct SpliceResult;

class MarkerIndex {
  friend class Iterator;
 public:
  MarkerIndex(unsigned seed);
  int GenerateRandomNumber();
  void Insert(MarkerId id, Point start, Point end);
  void SetExclusive(MarkerId id, bool exclusive);
  void Delete(MarkerId id);
  SpliceResult Splice(Point start, Point old_extent, Point new_extent);
  Point GetStart(MarkerId id) const;
  Point GetEnd(MarkerId id) const;
  std::set<MarkerId> FindIntersecting(Point start, Point end);
  std::set<MarkerId> FindContaining(Point start, Point end);
  std::set<MarkerId> FindContainedIn(Point start, Point end);
  std::set<MarkerId> FindStartingIn(Point start, Point end);
  std::set<MarkerId> FindEndingIn(Point start, Point end);

 private:
  Point GetNodeOffset(const Node *node) const;
  void DeleteNode(Node *node);
  void BubbleNodeUp(Node *node);
  void BubbleNodeDown(Node *node);
  void RotateNodeLeft(Node *pivot);
  void RotateNodeRight(Node *pivot);
  void GetStartingAndEndingMarkersWithinSubtree(const Node *node, std::set<MarkerId> *starting, std::set<MarkerId> *ending);
  void PopulateSpliceInvalidationSets(SpliceResult *invalidated, const Node *start_node, const Node *end_node, const std::set<MarkerId> &starting_inside_splice, const std::set<MarkerId> &ending_inside_splice, bool is_insertion);

  std::default_random_engine random_engine;
  std::uniform_int_distribution<int> random_distribution;
   Node *root;
  std::map<MarkerId, Node*> start_nodes_by_id;
  std::map<MarkerId, Node*> end_nodes_by_id;
  Iterator iterator;
  std::unordered_set<MarkerId> exclusive_marker_ids;
};

#endif // MARKER_INDEX_H_
