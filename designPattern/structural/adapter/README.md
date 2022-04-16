# Quick fact
- Adapter:
    - we are given an interface, but we want a different one, and building an adapter over the interface is what gets us to where we want to be.
    - Namely adapter becomes a bridge between two incompatible interfaces. This pattern involves a single class called adapter which is responsible for communication between two independent or incompatible interfaces.
- The only real issue with adapters is that, in the process of adaptation, you sometimes end up generating temporary data so as to satisfy some other representation of data.
- To ensure that new data is only generated when necessary. you need to cache. But then you’ll need to clean up stale data when the cached objects have changed.
- Potentially, you might want to do it in a lazy way: only want the work to be done when the adapter is actually used?