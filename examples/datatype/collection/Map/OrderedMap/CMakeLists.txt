# OrderedMap<K,V> tests (BTree implementation, O(log n) operations)
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapBasicOperations         OrderedMapBasicOperations.cpp)
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapAddUpdate               OrderedMapAddUpdate.cpp)
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapIndexAccess             OrderedMapIndexAccess.cpp)
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapFind                    OrderedMapFind.cpp)
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapIterators               OrderedMapIterators.cpp)
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapEnum                    OrderedMapEnum.cpp)
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapEdgeCases               OrderedMapEdgeCases.cpp)
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapFunctionalOrdering      OrderedMapFunctionalOrdering.cpp)

# Move Semantics Test
cm_example_project("DataType/Collection/Map/OrderedMap" OrderedMapMoveSemantics            OrderedMapMoveSemantics.cpp)
