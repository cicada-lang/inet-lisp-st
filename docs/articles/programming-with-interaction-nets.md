---
title: Programming with Interaction Nets
author: Xie Yuheng
year: 2023
---

# 1

At the end of 2021,
I occasionally read a 1990 paper "Interaction Nets",
by Yves Lafont.
The paper introduced a very interesting new computation model,
using graph consists of nodes and edges as data,
and viewing interaction between connected nodes as computation.

In this paper, I will follow Lafont's examples
to introduce the principal of interaction nets.
And I will also introduce a language I designed
to program this computation model.

# 2

How to use graph to encode data?

Suppose we want to encode the simplest data -- natural number.
We can mimic the ancient knot counting, using node to do the counting.

```
0  (zero)-
1  (zero)-(add1)-
2  (zero)-(add1)-(add1)-
3  (zero)-(add1)-(add1)-(add1)-
```

The node representing 0 `(zero)-` has one port,
the node representing +1 `-(add1)-` has two ports,
we can encode natural number
by connecting these nodes through the ports.

# 3

How to use graph to represent functions that operate on natural numbers?

Take addition as an example, we need to introduce a new node to represent addition,
and to define interaction rules between this node and other nodes.

We use a node with three ports to represent addition.

```
       |
     (add)
     /   \
```

The two ports below represent the input `target` number and the `addend`,
the port above represent the output `value`.

```
     value
       |
     (add)
     /   \
 target  addend
```

We can represent 0 + 1 as the following:

```
       |
     (add)
     /   \
(zero)   (add1)
           |
         (zero)
```

and 2 + 2 as the following:

```
       |
     (add)
     /   \
(add1)   (add1)
  |        |
(add1)   (add1)
  |        |
(zero)   (zero)
```

By defining the interaction rules between `(add)` and neighbor nodes,
we can do addition.

When the `target` port of `(add)`is connected with `(zero)`,
delete `(zero)` and `(add)`,
and connect the `value` of `(add)` with the `addend` of `(add)` directly.

```
     value           value
       |               |
     (add)     =>      |
     /   \              \
(zero)   addend        addend
```

When the `target` port of `(add)` is connected with `(add1)`,
move `(add1)` above `(add)`.

```
     value           value
       |               |
     (add)     =>    (add1)
     /   \             |
(add1)   addend      (add)
  |                  /   \
prev              prev   addend
```

By these two interaction rules, the graph representing 2 + 2
will become 4 through the following interaction:

```
       |                  |                 |            |
     (add)              (add1)            (add1)       (add1)
     /   \                |                 |            |
(add1)   (add1)         (add)             (add1)       (add1)
  |        |    =>      /   \      =>       |       =>   |
(add1)   (add1)   (add1)    (add1)        (add)        (add1)
  |        |         |        |           /   \          |
(zero)   (zero)   (zero)    (add1)    (zero)  (add1)   (add1)
                              |                 |        |
                            (zero)            (add1)   (zero)
                                                |
                                              (zero)
```

# 4

Let's design a programming language
to program this computation model.

In our language each node has fixed number of ports.

```
(zero) -- has one port
(add1) -- has two ports
(add)  -- has three ports
```

Every port has a name.

```
(zero)-value  -- the value of 0

(add1)-prev   -- previous number
(add1)-value  -- the value of +1

(add)-target  -- target number
(add)-addend  -- the number to be added
(add)-result  -- result of addition
```

There are two kinds of ports -- input ports and output ports.

```
(zero)-value   -- output port

(add1)-prev    -- input port
(add1)-value   -- output port

(add)-target   -- input port
(add)-addend   -- input port
(add)-result   -- output port
```

Two nodes can be connected through ports.

Take the graph representing 2 as an example:

```
(zero)-(add1)-(add1)-
```

The detailed connections are the following:

```
(zero)-value-<>-prev-(add1)
(add1)-value-<>-prev-(add1)
(add1)-value-<>-
```

Each node has one and only one principal port,
two nodes can interact only when they are
connected through two principal ports.

```
(zero)-value!   -- principal port

(add1)-prev
(add1)-value!   -- principal port

(add)-target!   -- principal port
(add)-addend
(add)-result
```

We design the statement to define node as follows:

- The statement starts with `define-node`, follows the name of the node.
- Use `!` suffix to mark principal port.

The aforementioned nodes are defined as follows:

```
(define-node zero value!)
(define-node add1 prev value!)
(define-node add target! addend result)
```

# 5

Given two nodes, we can define an interaction rule for them.

Let's review the interaction rule between `(add1)` and `(add)`:

```
     result          value
       |               |
     (add)     =>    (add1)
     /   \             |
(add1)  addend       (add)
  |                  /   \
prev            target   addend
```

We can see that, the so called interaction can be viewed as:

- Remove the edge between the two principal ports.
- Remove the two nodes matched by the rule, at this point, the ports
  originally connected to these two nodes will be exposed.
- Reconnect the exposed ports, during which we can introduce new nodes.

We design the statement for defining rule as follows:

- The statement starts with `define-rule`, follows the pattern that matches a sub-graph.
- Use the function application syntax to build graph.

The the rule between `(add1)` and `(add)` as an example:

```
(define-rule (add (add1 prev) addend result)
  (add1 (add prev addend) result))
```

The rule between `(zero)` and `(add)` is a little special,
because during reconnecting the exposed ports,
it does not introduce any new nodes.

```
(define-rule (add (zero) addend result)
  (connect addend result))
```

# 6

Using the statements designed above,
we can write a complete code example.

In which we will use `define` to define new words,
and use `--` to comment a line.

```
(define-node zero value!)
(define-node add1 prev value!)
(define-node add target! addend result)

(define-rule (add (zero) addend result)
  (connect addend result))

(define-rule (add (add1 prev) addend result)
  (add1 (add prev addend) result))

;; test

(define (two) (add1 (add1 (zero))))

(inspect-run (add (two) (two)))
```

# 7

We emphasize the constraints of interaction nets, as a computational
model some of the good properties of interaction nets are gained by
these constraints.

The first constraint is, given two nodes,
we can define at most one interaction rule.

That is to say, when we find two nodes are connected through two
principal ports, either we can not find a rule for these two nodes,
then the two nodes can not interact; or we can find one and only one
rule, the two nodes will interact according to this rule.

This constraint excluded the case of finding multiple rules,
and need to making choice between them.

The second constraint is, each node has one and only one principal port.

Suppose two nodes are connected through two principal ports.
We draw a circle to enclose these two nodes and the edge between the principal ports.
Because each node has one and only one principal port,
all edges can go across the circle are not edge connecting principal ports.
These kind of edges can not interact at all.

```
     \   |   /
  .-------------.
  |    \ | /    |
  |   (.....)   |
  |      |      |
  |   (.....)   |
  |    / | \    |
  `-------------`
     /   |   \
```

Although during an interaction between two nodes, new nodes and new
interactable edges might be introduced, all of the new interactable
edges can still be viewed as contained within the circle, during all
the new future interactions caused by them, removing and reconnecting
will not affect other parts of the graph outside the circle.

That is to say, in interaction nets,
all interactions are independent,
first do interaction here or there
will not affect the final result of the computation.

If the sequence of interactions at different place is ignored,
then in interaction nets, not only the result of the computation
is unique, the process of computation is also unique!

When implementing interaction nets,
if the computer as multiple cores,
we can start multiple threads,
sharing the same memory,
do the interactions at different place in parallel,
the threads will not interfere with each other.

# 8

Every node has one and only one principal port,
this constraint can bring good properties to our computation model,
but it also make programming inconvenient.

The max function of natural number is an example of such inconvenience.
Let's introduce a node `(nat-max)` for this function.

```
     result
       |
   (nat-max)
     /    \
first!   second
```

Node definition:

```
(define-node nat-max first! second result)
```

The interaction between `(zero)` and `(zero)` is simple:

```
     result         result
       |              |
   (nat-max)      =>  |
     /    \            \
(zero)   second       second
```

Rule definition:

```
(define-rule (nat-max (zero) second result)
  (connect second result))
```

For the `(add1)` and `(zero)`,
if there is no single-principal-port constraint,
we can imagine the following interaction:

```
     result           result
       |                |
   (nat-max)    =>    (add1)
     /    \             |
(add1)    (add1)    (nat-max)
   |        |         /   \
 prev      prev    prev   prev
```

But because of single-principal-port constraint,
we have to introduce an auxiliary node and some auxiliary rules,
to explicitly choose between two interactable edges.

We call the auxiliary node `(nat-max-add1)`.

```
     result
       |
  (nat-max-add1)
     /    \
first    second!
```

Node definition:

```
(define-node nat-max-add1 first second! result)
```

Using the auxiliary node to define
the rule between `(add1)` and `(nat-max)`:

```
     result             result
       |                  |
   (nat-max)     => (nat-max-add1)
     /    \             /   \
(add1)   second      prev   second
   |
 prev
```

Rule definition:

```
(define-rule (nat-max (add1 prev) second result)
  (nat-max-add1 prev second result))
```

The rule between `(zero)` and `(nat-max-add1)`:

```
     result            result
       |                 |
 (nat-max-add1)    =>  (add1)
     /    \              |
 first   (zero)        first
```

Rule definition:

```
(define-rule (nat-max-add1 (zero) first result)
  (add1 first result))
```

The rule between `(add1)` and `(nat-max-add1)`:

```
     result            result
       |                 |
 (nat-max-add1)   =>   (add1)
     /    \              |
 first   (add1)      (nat-max)
           |           /   \
          prev     first   prev
```

Rule definition:

```
(define-rule (nat-max-add1 (add1 prev) first result)
  (add1 (nat-max first prev) result))
```

```
(define-node nat-max first! second result)
(define-node nat-max-add1 first second! result)

(define-rule (nat-max (zero) second result)
  (connect second result))

(define-rule (nat-max (add1 prev) second result)
  (nat-max-add1 prev second result))

(define-rule (nat-max-add1 (zero) first result)
  (add1 first result))

(define-rule (nat-max-add1 (add1 prev) first result)
  (add1 (nat-max first prev) result))

(inspect-run (nat-max (one) (two)))
```

# 9

We have already analyzed the node representing addition `(add)`,
now we analyze the node representing multiplication `(mul)`.

We will find that, to define the interaction rule between `(mul)` and
`(zero)` or `(mul)` and `(add1)`, we need to introduce auxiliary nodes
again:

- `(nat-erase)` to erase a natural number.
- `(nat-dup)` to duplicate a natural number.

These two nodes are different from all aforementioned nodes,
because all aforementioned nodes has one output port,
but:

- `(nat-erase)` has zero output ports.
- `(nat-dup)` has two output ports.

This is the main reason why we use stack to build net.

The good thing about using stack to pass arguments is that,
it can naturally handles zero-return-value and multiple-return-values,
we do not need to design new special syntax for these cases.

After decide to use stack to build net,
we can go one step further to use pure postfix notation as syntax.
This give us another good thing, i.e. composition of words is associative.
Thus when we want to factor out a subsequence from a sequence of words,
there will be no complicated syntax preventing us from doing so.

```
(define-node nat-erase target!)

(define-rule (nat-erase (zero)))
(define-rule (nat-erase (add1 prev)) (nat-erase prev))

(define-node nat-dup target! first second)

(define-rule (nat-dup (zero) first second)
  (connect first (zero))
  (connect second (zero)))

(define-rule (nat-dup (add1 prev) first second)
  (= prev-first prev-second (nat-dup prev))
  (connect first (add1 prev-first))
  (connect second (add1 prev-second)))

(define-node mul target! mulend result)

(define-rule (mul (zero) mulend result)
  (nat-erase mulend)
  (zero result))

(define-rule (mul (add1 prev) mulend result)
  (= mulend-first mulend-second (nat-dup mulend))
  (add (mul mulend-second prev)
       mulend-first
       result))

(inspect-run (mul (two) (two)))
```

# 10

After introduced the simplest data natural number,
we introduce the second simplest data -- list.

The goal is to implement `append` function.

The `(append)` of list
is very similar to the `(add)` of natural number.
The difference is that the `(add1)` of natural number only add one node,
while the `(cons)` of list add one node and link to an extra node.

```
(define-node null value!)
(define-node cons head tail value!)
(define-node append target! rest result)

(define-rule (append (null) rest result)
  (connect rest result))

(define-rule (append (cons head tail) rest result)
  (cons head (append tail rest) result))

;; test

(define-node sole value!)

(inspect-run
  (append
    (cons (sole) (cons (sole) (cons (sole) (null))))
    (cons (sole) (cons (sole) (cons (sole) (null))))))
```

# 11

If we want to use `(append)` to append two lists,
we must traverse the `target` of `(append)`,
while building a new list step by step,
and appending it to the front of the `rest` of `(append)`.

Do it in this way, the steps required to append two lists
is proportional to the length of the first list.
Is there a way to directly connect the end of the first list
to the start of the second list?
Which only requires fixed number of steps to append two lists.

We can define a new type diff-list,
and a new node `(diff)`，
this node can be used to hold the front and the back of a list.
If we want to append two diff-lists,
we can simply connect the back held by the first `(diff)`,
to the front held by the second `(diff)`.

Note that, in common programming languages,
we often use tree like expressions as data,
from a parent node we can find the children nodes,
while the reverse is not true.
But in interaction nets,
the relationship between all nodes is symmetric.

```
(define-node diff front back value!)
(define-node diff-append target! rest result)
(define-node diff-open new-back target! old-back)

(define-rule (diff-append (diff front back) rest result)
  (= new-back value (diff front))
  (connect value result)
  (diff-open new-back rest back))

(define-rule (diff-open (diff front back) new-back old-back)
  (connect back new-back)
  (connect front old-back))

;; test

(define (sole-diff-list)
  (= front front-op (link))
  (= back value (diff front))
  (cons (sole) (cons (sole) back) front-op)
  value)

(inspect-run
 (diff-append
  (sole-diff-list)
  (sole-diff-list)))
```

# 12

It is the end of this article now.

Let's look back, and look forward.

## Parallel Computing

Interaction nets as a computation model is interesting in deed,
in which every step of computation can be performed independently,
therefore it is very suitable for parallel computing.

## Syntax for Nonlinear Computational Models

Using stack and postfix notation to build net,
give us a simple syntax for interaction nets.

In fect, for graph-based computation models like interaction nets,
the graph itself is the syntax.
But graph is nonlinear, how to use linear text to describe graph?
We solve this by using stack and postfix notation to build graph.

In this way, the language we used to build graph,
becomes the lower layer language for the language of interaction nets.

## To be a Practical Programming Language

In pure interaction nets, the only data are
graphs consist of nodes and edges,
to encode natural number we need to do something like knot counting,
in many use cases, this is obviously not practical.

We can make the language a practical programming language,
by extending it with primitive datatypes like int and float.

How to design such extension?
Please wait for my next report :)
