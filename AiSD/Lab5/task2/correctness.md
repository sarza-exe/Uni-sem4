# Broadcast Scheduling in a Rooted Tree

## Problem statement (telephone broadcast on trees):
Given a rooted tree (root = 0) where the root initially knows some information.
In each synchronous round, every informed node can inform exactly one of its children.
We want to compute, for each node, the order in which it should inform its children
so that the total number of rounds needed to inform all nodes in the tree is minimized.

## Algorithm overview:
1. Construct the rooted tree (adjs + parent → children).
2. Perform a post-order DFS: for each node u, compute t[u] = minimum rounds needed
   for the subtree rooted at u (assuming u is informed at time 0).
3. To compute $t[u]$:
   - Let $C = {c1, c2, ..., ck}$ be the children of u.
   - Recursively compute $t[c_i]$ for each child $c_i$.
   - Sort children in descending order of $t[c_i]$.
   - Now schedule: $u$ informs children in that sorted order, one per round.
      The child ordered first (largest subtree time) is informed at round 1, second child at round 2, etc.
   - For each child in position i (1-based), the time that subtree $c_i$ finishes is $i + t[c_i]$.  i rounds until child $c_i$ gets informed, plus $t[c_i]$ within its subtree.
   - $t[u] = max_{i=1..k} (i + t[c_i])$.  If u has no children, $t[u] = 0$.

This greedy ordering is optimal because giving priority (informing earlier) to children
with larger subtree times reduces the global maximum completion time. It can be proved
by an exchange argument: if two children are out of order (smaller subtree time scheduled
before larger), swapping them cannot increase the maximum finish time, and will often
reduce it.

## Correctness:
We want to minimize the time T until the last node in the tree is informed. Observe that
once a node u is informed, it will eventually need to inform all of its children, each of
which then starts informing their subtrees. In each round, u can choose at most one
child to inform.

Let $t[u]$ be the minimum number of rounds needed to inform all nodes in the subtree rooted at $u$, assuming $u$ is informed at time $0$. If $u$ is a leaf, then $t[u]=0$ (no children to inform). If $u$ has $k$ children $c_1, c_2, ..., c_k$, and if we choose in which rounds to inform each child, we can model the finish time of each child's subtree as follows: if $u$ informs $c_i$ at round $r_i$ ($1 ≤ r_i ≤ k$, and all $r_i$ are distinct), then the subtree rooted at $c_i$
will require $t[c_i]$ additional rounds, so the finish time for that child's entire subtree is $r_i + t[c_i]$. 

We want to minimize the maximum over $i$ of $(r_i + t[c_i])$. Since each child must be informed in a separate round (and we must schedule $k$ distinct integers in ${1,2,...,k}$), the best strategy is to assign the smallest $r_i$ values (i.e., 1, 2, 3, ...) to children with larger $t[c_i]$. 

In other words, we sort children by descending $t[c_i]$, and inform them in that order. This is the classic greedy exchange argument: if two children $c_a, c_b$ are scheduled out of order (i.e., child with smaller $t$ is informed before child with larger $t$), then swapping their informing rounds can only improve (or keep equal) the maximum finish time. Therefore, the greedy sort-by-descending-t yields the minimum possible $t[u] = max_{i=1..k}$  $(i + t[ordered_child_i])$.

Once we compute $t[u]$ for all $u$ via a post-order DFS, the value $t[0]$ at the root is the
minimum number of rounds to broadcast to the entire tree. We also record the sorted
order of children at each node as $schedule[u]$, which gives the exact order in which
$u$ should inform its children to achieve this optimal time. Building $children[]$ via a
rooting DFS ensures that we treat the MST as a rooted tree with no cycles or back-edges.

## Complexity:
- Building the adjacency list and rooting the tree: $O(n)$.
- Computing $t[u]$ via a DFS: each node u sorts its list of children by their $t[c]$.
  If $deg(u) = d_u$, sorting takes $O(d_u log d_u)$. Summing over all nodes, total time
  is $\sum_{u}$ $O(d_u log d_u)$. Since $\sum{d_u} = n-1$ (total edges), and each $d_u <= n$,
  worst-case sorting time is $O(n log n)$ (e.g., if the tree is a star, one node has
  degree n-1, so sorting takes $(n-1)*log(n-1))$.
- Therefore, the overall time complexity is $O(n log n)$.
- Space complexity: $O(n)$ for adjacency lists, $O(n)$ for storing $t[u]$, and $O(n)$ for schedules.

------------
Total (dominated by MST generation): $O(n^2 log n)$ for dense graphs. If MST is already
provided, then the scheduling algorithm runs in $O(n log n)$. The space usage is $O(n^2)$
to store the complete graph (for MST), $O(n)$ for all tree structures and arrays.

