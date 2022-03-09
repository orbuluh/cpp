# Quick fact
* Observable is observed by Observer - e.g. When observable change, it would trigger observer to react.


# Food for thought
## Dependency Problems
- Imaging observable is an Excel cell, and observable is an Excel calculation
- Given lots of dependencies between different cells, how do you know which cells to recalculate when one of them changes.
- Relationships between observer ('s reaction) and potential change for the observable needs to be tracked manually.

## Unsubscription and Thread Safety
- `subscribe()` and `unsubscribe()` of the observable might be racy. Adding a mutex is an easy solution.
- or potentially doing something like null'ing the entry but not removing it
```cpp
void unsubscribe(Observer<T>* o) {
    auto it = find(observers.begin(), observers.end(), o);
    if (it != observers.end())
    *it = nullptr; // cannot do this for a set
}

void notify(T& source, const string& name) {
    for (auto obs : observers) {
        if (obs)
            obs->field_changed(source, name);
    }
}
```

## Potential dead lock - a.k.a Reentrancy
- Observable change trigger observer to act, which triggers observable to do something --> could be a dead lock for the observable.

## Others...
- What happens if the same observer is added twice?
- If I allow duplicate observers, does `unsubscribe()` remove every single instance?
- How is the behavior affected if we use a different container for observers?
- What if I want observers that are ranked by priority? ...etc
- If you don’t like the idea of dispatching on a (posibly null) raw pointer, consider using a weak_ptr instead.
