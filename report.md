# FILE NAME: StudentID_HW4.pdf <!-- TODO pls name correctly when submit>

## Student / Team Information
both names, both student IDs, and the chosen seed student ID
missing name / student ID / chosen seed information will result in a 3-point penalty

## Implementation Summary
what you completed in Task 1, Task 2, and Task 3 <!-- TODO Huh??


## Address Mapping Explanation
how you compute block offset, set index, and tag <!-- TODO pls check -->
- block offset refers to the bottom bits that should be ignored for the entire address to be a multiple of block_size bytes. Hence log2(block_size)
- set index number of bits needs to be enough to address the full range of the sets. Hence we need log2(number_of_sets) bits, where number_of_sets is just the total number of bytes in the cache divide by the bytes in each set. The bytes in each set = associativity * block_size. Then we just take set_index number of bits from the right after shifting the address right by block_offset_bits beforehand
- tag is all bits remaining after logical shift right by (set_index_bits + block_offset_bits)

how these change when cache geometry changes <!-- TODO pls check -->
- block offset just depends on block_size. Even if associativity changes, we only care about the size inside one block in the set, not the whole set
- <!-- TODO set index>
- <!-- TODO tag>

## Task 1 Testing
how you tested correctness under multiple cache configurations
representative results

## Task 2 Hierarchy Explanation
how L1 , L2 , and memory interact
- we always start searching for data in L1
- L1 either returns if present, or consults L2
- L2 responds by returning data if present, or consults Main Memory
- Main Memory definitely has the data and will return it to L2
- L2 will pass it to L1, and L1 pass it to the CPU to be processed
- each time we consult next level in hierarchy, data latency increases

what changed after adding L2
- as seen in the difference of task1 and task2 MAKEFILE output
- the total accesses of Main Memory was significantly reduced because L2 helped to serve the misses from L1
- the total AMAT also dropped because L2 can accomodate more data and still be faster than Main Memory to provide the data to L1
- without L2, we would have to pay Main Memory data latency for every L1 miss
- with L2, we have more chance of finding the data in a relatively faster L2 cache before consulting Main Memory and paying the long latency <!-- TODO

## Task 3 Design Choices
which replacement policies and prefetchers you implemented
whether you designed your own prefetcher

## Trace Analysis
which student ID was used as the trace-generation seed <!-- TODO bry urs

what access patterns you observed in your personalized trace 
how those patterns influenced your design decisions

0. Read Dominated <!-- TODO i mean we don't really care about this right?

1. Poor Temporal Locality
- modified the python script to count the number of zero block deltas
- there was only around 100 *zero block deltas* in more than 8000 accesses
- hence, no need to worry too much about polluting cache because it's unlikely we will need old data <!-- TODO gini ta?
- that is why we prefetch aggressively for Stride Prefetcher (threshold is 1 instead of 2), and sample a relatively huge region for Region Prefetcher before (not so large that it becomes detrimental to us) <!-- TODO check pls
Most frequently accessed blocks
  4199213: 60 (0.71%)
  4199277: 60 (0.71%)
  4199341: 60 (0.71%)
  4199405: 60 (0.71%)
  4199469: 60 (0.71%)
  4199533: 60 (0.71%)
  4199597: 60 (0.71%)
  4199661: 60 (0.71%)
- the analysis also shows *most frequent access percentage* still less than 1%
- confirming the poor temporal locality of the trace <!-- TODO help

2. Stride Dominated
Top block strides (in cache blocks)
  7: 4052 (48.09%)
  1: 2262 (26.85%)
  64: 1434 (17.02%)
- these 3 strides make up most of the trace, other strides less than 1% each
- that is why L1 stride prefetches the next 2 blocks aggressively so that we can leverage the fast latency of L1, instead of L2 as the stride prefetcher <!-- TODO

3. Multiple Blocks in One Set
Per-window summary (window size = 256 accesses)
  [  2304,   2560) reads= 256 writes=   0 unique_blocks=  23 unique_sets=   2
  [  2560,   2816) reads= 256 writes=   0 unique_blocks=  14 unique_sets=   1
  [  3840,   4096) reads= 243 writes=  13 unique_blocks=  20 unique_sets=   1
  [  4096,   4352) reads= 238 writes=  18 unique_blocks= 160 unique_sets=  64
  [  4352,   4608) reads= 249 writes=   7 unique_blocks= 249 unique_sets=  64
- 20 unique blocks into 1 unique set, means we need ASSOC > 20 to accomodate and prevent cache trashing, otherwise L1 misses will increase if we keep throwing out useful data
- nevertheless, we have L2 which would account for the misses in L1. But having high ASSOC would really benefit L1 to have higher hit rate

--- <!-- TODO for some reason, 8-way is worse than 1-way???????
Constructed L2: 128KB, 8-way, 4cyc, [LRU + Region]
Constructed L1: 32KB, 8-way, 1cyc, [LRU + Stride]

=== Starting Simulation ===

=== Simulation Results ===
  [L1] Hit Rate: 79.79% (Access: 8427, Miss: 1703, WB: 216)
      Prefetches Issued: 8053
  [L2] Hit Rate: 99.77% (Access: 9972, Miss: 23, WB: 7)
      Prefetches Issued: 134860
  [Main Memory] Total Accesses: 134890

Metrics:
  Total Instructions: 8427
  Total Cycles:       16839
  AMAT:               2.00 cycles
---

--- <!-- TODO 1-way doesn't miss as much as 8-way L1 wtf??
Constructed L2: 128KB, 1-way, 4cyc, [LRU + Region]
Constructed L1: 32KB, 1-way, 1cyc, [LRU + Stride]

=== Simulation Results ===
  [L1] Hit Rate: 90.41% (Access: 8427, Miss: 808, WB: 180)
      Prefetches Issued: 5646
  [L2] Hit Rate: 99.62% (Access: 6634, Miss: 25, WB: 21)
      Prefetches Issued: 7693
  [Main Memory] Total Accesses: 7739

Metrics:
  Total Instructions: 8427
  Total Cycles:       12659
  AMAT:               1.50 cycles
---

- but too high ASSOC would hurt L2 <!-- TODO hmm why, the testing shows it
=== Simulation Results === (ASSOC = 128)
  [L1] Hit Rate: 98.09% (Access: 8427, Miss: 161, WB: 162)
      Prefetches Issued: 6253
  [L2] Hit Rate: 88.12% (Access: 6576, Miss: 781, WB: 1) <!-- TODO why L2 ded
      Prefetches Issued: 116177
  [Main Memory] Total Accesses: 116959

4. Occasional Access of Data Outside Stride Pattern
- by the time it accesses a data with irregular pattern (occasionally), that data would probably not be inside L1 because L1 has been stride prefetching
- that is why we allow L2 to compensate by storing the data in between the strides, using Regional Prefetcher
<!--TODO below ini bener ga?-->
- when L1 consults L2, it will cause L2 to prefetch and area around the data requested. This works because L1 would most likely request data in regular strides and L2 would eventually contain the irregular data between the stride lengths. So if L1 ever needs to access those irregular pattern data, L2 got it
- prefetching in bulk also doesn't hurt latency because it occurs asynchronously, so we can leverage this Regional Prefetcher while being careful not to pollute L2 too much (systematic testing gives 128B as optimal)
=== Simulation Results === (prefetching in bulk: lots of memory access but we don't pay the memory latency because prefetching is asynchronous. Leveraging this gives us near perfect Hit Rate for L2)
  [L1] Hit Rate: 98.09% (Access: 8427, Miss: 161, WB: 162)
      Prefetches Issued: 6253
  [L2] Hit Rate: 99.44% (Access: 6576, Miss: 37, WB: 1)
      Prefetches Issued: 76003
  [Main Memory] Total Accesses: 76041

<!-- TODO below is using NextLine, it misses because we don't fetch enough?
the Hit Rate reduces slightly for L2 using Stride Prefetcher -->
=== Simulation Results === (without Regional Prefetch, L2 would miss more)
  [L1] Hit Rate: 98.09% (Access: 8427, Miss: 161, WB: 162)
      Prefetches Issued: 6253
  [L2] Hit Rate: 73.02% (Access: 6576, Miss: 1774, WB: 1)
      Prefetches Issued: 0
  [Main Memory] Total Accesses: 1775


<!-- TODO we should probably organize like this? idk -->
SUGGESTED PATTERN FROM BLACKBOARD:
-> observation from trace analysis
-> hypothesis about cache behavior
-> policy / prefetcher choice
-> experimental result that supports or rejects the hypothesis

## Experimental Results
tables comparing configurations <!-- TODO honestly idk what this is for

## Best Configuration and Discussion
your best-performing design
why it performs well
where it may still fail <!-- TODO where will it fail? I guess if there was no stride at all? But L2 would still cover.. lmao it's never gonna fail?

## External Resources and AI Usage
list any external resources you relied on, including websites, textbooks, friends, or LLMs
if you use LLM tools such as ChatGPT, include the conversation link at the end of your report
<!-- whose link to include? I mostly used deepseek for Github :skull:>
