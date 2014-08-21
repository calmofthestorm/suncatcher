=================
Suncatcher
=================

A library for efficiently computing shortest paths and reachability in predictably dynamic statespaces.

| Alex Roper
| alex@aroper.net
| http://github.com/calmofthestorm

-------------------------------------------------------------------------------------------

Summary
-------

Suncatcher is a library for efficiently computing reachability and shortest paths in dynamic statespaces, intended for use in modeling with many independent agents and large simulation video games. By operating on an abstraction of the world where a transition may be not only passible or impassible but also hinted as "frequently changing" (i.e., a "door"), we can take advantage of hints to improve performance in expected cases while always returning correct results.

Features:

* Pay only for what you use design emphasizing both performance and flexibility as the user requires.
* Fast API based on concrete classes for a cache-efficient abstraction of 3D Euclidean statespaces.
* Flexible polymorphic API for defining arbitrary statespaces.
* Optional, efficient copy-on-write semantics allow for predictivie mutation and pathing -- put spare cores to work!
* Thread safe implementation to make predictive planning easy (not yet tested thoroughly).
* Incremental update postpones expensive recomputations where possible, and avoids worst case behavior except in actual worst cases.
* Emphasis on logical, rather than bitwise, constness to allow for higher level reasoning about behavior.
* Modern API based on C++11, RAII, and immutable value semantics (where possible) to make resource management efficient, easy, and fun, even when using multiple state spaces and doing predictive computation.
* Thorough test suite compiles clean and runs clean under Valgrind. All warnings and errors, g++ and valgrind, are taken seriously.
* Cross platform build managed by CMake works with clang and g++ on Linux and vs2013 on Windows. g++ on cygwin and mingw support is planned.

Current Status
--------------

Still in early development as a side project, with major changes expected down the line.

Overview
--------

The core algorithm, in brief:

- Operate on an abstraction of the state space rather than the state space itself to improve cache locality.
- Use connected components to compute constant time path existence.
- Tag some links as likely to be added/removed for more efficient treatment.
- Incrementally update connected components wherever possible.

The pathological case of optimal shortest paths algorithms is when no path exists, in which case the runtime is linear in the number of states reachable from the start and/or goal state, which is prohibitive for all but the smallest statespaces when many paths need to be computed. A single linear pass to split the world into connected components is a common optimization in pathfinding to address this -- a path exists from start to finish if and only if they are in the same connected component. In static (or rarely changing) state spaces, this combined with caching most frequently used portions of paths, is typically sufficient to achieve acceptable performance, possibly combined with approximate shortest path algorithms such as HPA*.

In dynamic state spaces, however, caching shortest paths is far less effective. Cache invalidation in response to incremental world changes can be comparably expensive to recomputing paths, depending on the change (for example, increasing the cost of a transition requires at a minimum partial recomputation of all shortest paths which include it). Incrementally updating connected components is easier -- adding a transition between two states simply unions them, which can be done efficiently. Removing a transition, however, requires recomputation of all connected components in the worst case. If a linear pass over the entire state space every mutation is unacceptable, this can be problematic.

D* and D*Lite are efficient algorithms for finding and following shortest paths in highly dynamic worlds, minimizing recomputation when the world changes by caching large parts of the initial search. However, they rely on a substantial amount of memory being available to each agent, and scale poorly when computing paths is still frequent compared to mutation, as occurs with many independent agents being modeled by the same computer on a less frequently changing world, where an optimistic invalidation approach fares better.

The key design of Suncatcher is the idea that we often know in advance which transitions will be added, removed, and or change state. (For example, a door or gate that opens and closes, a time-parameterized link, etc.) If a few transitions do most of the changing from passable to impassible, and these are known upfront, we can efficiently keep track of information sufficient to answer reachability queries in constant time, and to change the state of a transition in time at worst linear in the number of these dynamic transitions -- essentially constant, compared to the size of the state space.

We do this by maintaining two levels of connected components -- a "static" label that treats all dynamic transitions as impassible, and labels equivalence classes that are always equivalent, and a "dynamic" label that takes into account the current state of dynamic transitions. These equivalence classes are efficiently maintained dynamically using a union find to allow amortized constant-time queries in most cases. When a dynamic transition is removed, we only need to recompute the dynamic labels, but when a non-dynamic transition is removed we need to recompute all labels. Adding a transition (dynamic or otherwise) can always be done in amortized constant time.

Example
-------

Consider as a simple example a 2D dungeon on a grid, made up of rooms, halls, etc. Each cell in the grid can be either a wall (*), an open door (d), a closed door (D), or open (walkable) space. For example::

**********
*        *
*  *d*   *
*  * *   *
*  ***   *
*        *
*  *D*   *
*  * *   *
*  *d*   *
*        *
*  ***   *
*  *     *
*  ***  d*
*        *
**d*******

The doors are the dynamic transitions, under the assumption that they frequently open and close, relative to changes to the rest of the map. Below are the static labels (indicated with numbers), which will only require expensive recomputation if we change an open space to a wall. This map has three static components: most of the map is marked 0, the two rooms are marked 1 and 2 respectively.::

**********
*00000000*
*00*d*000*
*00*1*000*
*00***000*
*00000000*
*00*D*000*
*00*2*000*
*00*d*000*
*00000000*
*00***000*
*00*00000*
*00***00d*
*00000000*
**d*******

Currently, since there is an open door between 0 and 1 and 0 and 2, the entire map is the same dynamic component -- a path exists between any two passable squares. If we were to close all doors, then the dynamic components would be the same as the static since we can't go through any doors.

Roadmap
-------

This is some basic notes on where this project is headed. No guarantees of course, neither on features nor timeline:-).

- Rewrite Euclidean state space to operate in terms of states and transitions (as the polymorphic API does), rather than the current design where each cell on the grid is either passable or impassible, and adjacent cells are connected. In practice this means a similar design, but admits the possibility of a wall between two cells not taking up a cell itself. The advantage is more flexible world modeling, better compatibility with the polymorphic API, simpler code, and better handling of 3D worlds in the presence of gravity. This will require substantial changes to the code and most tests.
- Consider removing path finding from the library's responsibility. Path finding is currently just straighforward A*, and our abstracted world provides no real advantage over what user code could do aside from (possibly) better cache performance depending on the user representation. By moving pathfinding into user code, we would also substantially improve the performance of the polymorphic API (which is currently about 30% slower because of the virtual get_adjacent calls in the A* and DFS tight loops.) Users could path on concrete data structures, and use Suncatcher only to maintain reachability. After doing so, it'd be worth re-running benchmarks to see if the concrete API is still justified.
- Currently enabling copy on write semantics reduces performance around 30% for both mutation and read-only queries. I suspect it may be possible to improve the performance for read-only queries. After doing so, re-evaluate if having a non-immutable API still makes sense.
- Make all code threadsafe. This should be mostly the case, as it was designed with thread safety in mind from the start, with the exception of the union find which either needs eager transitive closure or a locking scheme. Multithreaded test cases are also needed.
- Higher level API -- path invalidation observers, easy to use multithreaded speculative pathing/caching/mutation and invalidation, etc -- basically provide an API that looks like a single world, but is capable of taking hints and managing internal resources to speed up expected-case behavior. Currently all this complexity has to be handled by user level code, which may well make the difference between taking advantage of it and not.
- Support for different factions (doors passable to some but not others) and movement classes (flying vs walking) using the same state space abstraction. Currently you'd need one abstraction per faction * class, and a lot of that will be duplicated data.
- Support for "one-way" transitions based on a third abstraction layer using a directed acyclic graph over dynamic components (e.g., teleporter, jump off a roof, etc). This should be easy to do in linear time in the number of teleports, and add negligible cost for state spaces not using it.
- API documentation, examples, and polish
- Support for mingw and g++ on cygwin.

Can I use this?
---------------

In its current state? I wouldn't if I were you:-) The code is available under GPLv3. I'm open to considering a less restrictive copyleft licence (e.g., LGPLv3) if licensing is an issue -- I care more about people doing awesome things than ideological purity, including commercial and closed-source awesome things, but want to wait until it's done to think about the implications, in particular ensuring improvements make it back to Suncatcher, not just to a closed-source shim layer.
