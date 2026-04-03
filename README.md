# CSC3060 Project 4 - Cache Simulator
The fourth project of CSC3060 2026.

## Task Split - No Dependencies (I hope) Between Persons
### Task 1
Person A
- ~~Implement get_index() in memory_hierarchy.cpp~~

- ~~Implement get_tag() in memory_hierarchy.cpp~~

- ~~Implement hit detection logic in CacheLevel::access()~~

- ~~Implement miss handling (victim selection, write-back, fetch, install) in CacheLevel::access()~~

Person B
- ~~Implement reconstruct_addr() in memory_hierarchy.cpp~~

- ~~Implement LRU::onHit() in repl_policy.cpp~~

- ~~Implement LRU::onMiss() in repl_policy.cpp~~

- ~~Implement LRU::getVictim() in repl_policy.cpp~~

### Task 2
Person A
- ~~Add --enable-l2 flag parsing in main.cpp~~

- ~~Create L2 CacheLevel object in main.cpp~~

- ~~Connect L1 next_level pointer to L2 in main.cpp~~

- ~~Set L2 next_level pointer to Main Memory in main.cpp~~

Person B
- ~~Add L2 access counter variable in memory_hierarchy.cpp~~

- ~~Add L2 hit counter variable in memory_hierarchy.cpp~~

- ~~Add L2 miss counter variable in memory_hierarchy.cpp~~

- ~~Add L2 write-back counter variable in memory_hierarchy.cpp~~

- ~~Print L2 statistics in simulation output in memory_hierarchy.cpp~~

### Task 3
Person A
- ~~Implement SRRIP::onHit() in repl_policy.cpp~~

- ~~Implement SRRIP::onMiss() in repl_policy.cpp~~

- ~~Implement SRRIP::getVictim() in repl_policy.cpp~~

- ~~Implement StridePrefetcher::calculatePrefetch() in prefetcher.cpp~~

Person B
- ~~Implement BIP::onHit() in repl_policy.cpp~~

- ~~Implement BIP::onMiss() in repl_policy.cpp~~

- ~~Implement BIP::getVictim() in repl_policy.cpp~~

- ~~Implement NextLinePrefetcher::calculatePrefetch() in prefetcher.cpp~~

- ~~Implement install_prefetch() in memory_hierarchy.cpp~~

- ~~Call prefetcher from CacheLevel::access() in memory_hierarchy.cpp~~

### Finale
- Design our own prefetcher
