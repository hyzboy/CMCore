# Container Renaming Plan

This note proposes a first-pass categorization and renaming scheme for container/manager classes (excluding string types). It focuses on clarifying responsibilities, ownership, and strategy semantics; detailed API changes are out of scope.

## Vocabulary Conventions (proposed)
- **Ownership**: `Owned` (container owns/destroys), `Ref` (non-owning reference), `View` (borrowed, lightweight), `Handle` (ID/handle indirection).
- **Threading**: suffix `Sync` (internally synchronized), `LockFree` (lock-free), default is non-thread-safe.
- **Strategy**: `Pool` (reuse), `Arena` (bump/region allocator), `Cache` (eviction), `Flat` (contiguous / vector-backed), `Small` (small-cap optimized), `Mono` (monotonic growth), `Seg` (segmented/grow-by-chunk).
- **Container families**: `Array` (fixed-size), `List` (dynamic/resize), `Map`/`Set` (associative), `Queue`/`Stack` (FIFO/LIFO), `Tree`, `Pool`, `Cache`.

## Renaming Table (draft)
| Category | Current | Proposed | Notes |
| --- | --- | --- | --- |
| Memory / Pools | MemoryAllocator | Allocator | Base alloc interface/strategy hook |
|  | MemoryBlock | MemBlock | Block abstraction |
|  | Pool | ObjectPool | Generic object reuse |
|  | SeriesPool | SegPool | Segmented/grow-by-chunk pool |
|  | DataStackPool | StackPool | Stack-style pool semantics |
|  | RefObjectPool | RefPool | Non-owning or ref-counted pool variant |
|  | ActiveMemoryBlockManager | BlockPoolManager | Manages MemBlock pools |
|  | AccumMemoryManager | ArenaManager | Bump/arena-style allocator |
| Object / Lifetime | ActiveObjectManager | ObjectManager | Owns/dispatches objects |
|  | ActiveDataManager | DataManager | Data objects lifecycle |
|  | ActiveIDManager | IdManager | ID issuance/tracking |
|  | TickObject / object/TickObject | Tickable | Interface/contract |
|  | (manager for ticks) | TickScheduler | Manages ticking order |
| Sequence Containers | Array | Array | Fixed-size/value array |
|  | DataArray | Array | Trivial/value elements; pair with ObjectArray |
|  | ObjectArray | ObjArray | Non-trivial/owned objects; pair with DataArray |
|  | ArrayList | List | Trivial/value list; pair with ObjectList |
|  | ObjectList | ObjList | Non-trivial/owned list; pair with ArrayList |
|  | DataChain | Chain | Linked/chain structure |
|  | Queue | Queue | FIFO |
|  | Stack | Stack | LIFO |
|  | SeriesPool (if used as allocator) | SeqPool | If primarily sequence allocator |
| Associative / Sets | Map | Map | Default hash/ordered map |
|  | SmallMap | SmallMap | Keep name; small-size optimized |
|  | KeyValue | MapEntry / KeyValue | If only pair helper, or fold into Map |
|  | SortedSet | SortedSet | Ordered set |
|  | ConstStringSet | InternedSet | For interned/const strings |
|  | IndexedList | IndexMap | Index→value map |
|  | ElementCollection / Collection | Collection | Base collection concept |
| Special Strategy | LRUCache | LruCache | Cache with LRU eviction |
|  | FlatTree | FlatTree | Tree over flat storage |
|  | MonotonicIDList | MonoIdList | Monotonic ID growth list |
|  | BitsArray | BitArray | Bitset/bitmap |
|  | Gradient | GradientTable | Lookup table |
| Geometry / Bounds | Extent | Bounds2D | 2D extent |
|  | Size2 | Size2D | Width/height |
|  | AABB / AABB2D (in other dirs) | Bounds3D / Bounds2D | Axis-aligned bounds |
| String Adjacent (minimal) | StringView | StrView | Borrowed view |
|  | StringViewList | StrViewList | List of views |
|  | MergeString | StrJoin | Join utility |
|  | SplitString | StrSplit | Split utility |
| Utilities | Inherit | InheritTrait | Type trait helper |
|  | TypeInfo | TypeInfo | Keep name |
|  | UnicodeBlocks | UnicodeBlocks | Keep name |
|  | VersionData | VersionInfo | Standardize suffix |
|  | IDName | IdName | Consistent casing |
|  | Smart | SmartPtr | If smart-pointer wrapper |

## How to Use
- Treat this as a naming vocabulary map; it does **not** mandate immediate code moves.
- Before renaming, confirm per-type semantics: ownership, threading model, and allocator strategy.
- Apply names consistently within each family (sequence, associative, pool/cache).
- If two types collapse into one family (e.g., ObjectArray/ObjectList), decide ownership policy first, then pick the name.

## Pairing Notes
- `DataArray` ↔ `ObjectArray`: trivial/value vs non-trivial/owned objects.
- `ArrayList` ↔ `ObjectList`: trivial/value vs non-trivial/owned list.
- `Stack` ↔ `ObjectStack` (if present): value vs owned stack semantics.
- `Queue` ↔ `ObjectQueue` (if present): value vs owned queue semantics.

## Next Steps (optional)
- Lock in the vocabulary (prefix/suffix) in a short style guide for the module.
- Annotate each class with its ownership and threading policy to validate the proposed names.
- Identify actual duplicates/overlaps (e.g., ArrayList vs ObjectArray) and plan merges or aliases.
