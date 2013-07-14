cs378-deque
===========

Implement the class MyDeque to emulate std::deque

A **deque** is an attractive compromise between a **vector** and a **list**.

We are to implement a the class **MyDeque&lt;T, A&gt;** to conform to the same api as
[**std::deque&lt;T, A&gt;**](http://www.cplusplus.com/reference/deque/deque/).

Must meet the following requirements:
- **amortized constant** when adding elements to the beginning or end
- **linear** when adding elements to the middle
- **constant** when removing elements from the beginning or end
- **linear** when removing elements from the middle
- **constant** when indexing elements
- Must be implemented with an **array** of **arrays**

We **may not** use **new**, **delete**, **malloc()**, or **free()**.
We **may** use the **STL**. You **may not** use **std::deque&lt;T, A&gt;**.
