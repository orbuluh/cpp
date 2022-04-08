# Quick fact
- Iterator is a behavioral design pattern that lets you traverse elements of a collection without exposing its underlying representation (list, stack, tree, etc.).
- The main idea of the Iterator pattern is to extract the traversal behavior of a collection into a separate object called an iterator.
  - In addition to implementing the algorithm itself, an iterator object encapsulates all of the traversal details, such as the current position and how many elements are left till the end.
  - Usually, iterators provide one primary method for fetching elements of the collection. The client can keep running this method until it doesn’t return anything, which means that the iterator has traversed all of the elements.
  - All iterators must implement the same interface. This makes the client code compatible with any collection type or any traversal algorithm as long as there’s a proper iterator.
  - If you need a special way to traverse a collection, you just create a new iterator class, without having to change the collection or the client.
  - Implementing your own iterator is as simple as providing the ++ and != operators. Most iterators are simply pointer façades and are meant to be used to traverse the collection once before they are thrown away.

# Applicability
- Use the Iterator pattern when your collection has a complex data structure under the hood, but you want to hide its complexity from clients (either for convenience or security reasons).
- Use the pattern to reduce duplication of the traversal code across your app.
- Use the Iterator when you want your code to be able to traverse different data structures or when types of these structures are unknown beforehand.
  - The pattern provides a couple of generic interfaces for both collections and iterators. Given that your code now uses these interfaces, it’ll still work if you pass it various kinds of collections and iterators that implement these interfaces.

# Coroutines
- Coroutines fix some of the issues present in iterators: they allow state to be preserved between calls—something that other languages (e.g., C#) have implemented a long time ago. As a consequence, coroutines allow us to write recursive algorithms.