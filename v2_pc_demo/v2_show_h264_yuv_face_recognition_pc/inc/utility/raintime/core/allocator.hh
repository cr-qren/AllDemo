/*!\copyright
 * Copyright (c) 2017 Corerain Technologies. All rights reserved.
 * No part of this document, either material or conceptual may be
 * copied or distributed, transmitted, transcribed, stored in a retrieval
 * system or translated into any human or computer language in
 * any form by any means, electronic, mechanical, manual
 * or otherwise, or disclosed to third parties without
 * the express written permission of Corerain Technologies.
 */

#ifndef CORE_ALLOCATOR_HH_
#define CORE_ALLOCATOR_HH_

#include <stdlib.h>

#include <glog/logging.h>
#include <limits>
#include <stdexcept>

// The idea behind this allocator design is delighted by TensorFlow.

namespace raintime {

/*!
 * Base class for memory allocators.
 */
class Allocator {
 public:
  /*! constant of allocation alignment */
  static constexpr size_t kAllocAlign = 32;

  static Allocator *Global() { return nullptr; }

  /*!
   * Memory allocation in raw bytes with alignment.
   *
   * Example usage:
   * ```
   * Allocator allocator;
   *
   * void *p = AllocRaw(1024, 10);
   *
   * // p's address will be aligned to 1024-byte
   * ```
   *
   * \param align memory alignment
   * \param num_bytes number of bytes needed to be allocated
   * \return pointer to the allocated aligned memory
   */
  virtual void *AllocRaw(size_t align, size_t num_bytes) = 0;

  /*! Deallocate an allocated memory at pointer p with **raw** type.
   *
   * \param p pointer to a memory block that will be freed
   */
  virtual void FreeRaw(void *p) = 0;

  /*!
   * Allocate a memory block by number of elements.
   *
   * Use `T` to specify the type of elements which you will allocate.
   *
   * \param num_elems number of elements of type T to be allocated
   * \return pointer to the allocated memory
   */
  template <typename T>
  T *Alloc(size_t num_elems) {
    // check whether the number of elements to be allocated is too large
    // regarding numerical issue
    if (num_elems > (std::numeric_limits<size_t>::max() / sizeof(T))) {
      return NULL;
    }

    void *p = AllocRaw(kAllocAlign, num_elems * sizeof(T));

    // type cast
    T *typed_p = reinterpret_cast<T *>(p);

    return typed_p;
  }

  /*! Free a typed memory block.
   *
   * \param p typed pointer to an allocated memory space
   */
  template <typename T>
  void Free(T *p) {
    if (p) {
      FreeRaw(p);
    }
  }
};

/*!
 * An allocator uses default CPU `malloc`/`free` utilities.
 */
class DefaultCpuAllocator : public Allocator {
 public:
  static DefaultCpuAllocator *Global();

  void *AllocRaw(size_t align, size_t num_bytes) {
    // void* p = aligned_alloc(align, num_bytes);
    // simply ignore the align argument
    void *p = malloc(num_bytes);
    CHECK(p != nullptr);

    // TODO(vincent) might need add some statistic collect
    return p;
  }

  void FreeRaw(void *p) { free(p); }
};

/*!
 * Customised allocator assumes linear memory address growth.
 *
 * This allocator uses linked list to manage memory space.
 * It finds the frontmost memory block that has larger size
 * than required and use it to allocate space.
 * If the required space is less than the space within the
 * current block, the current block will split into two parts,
 * and only the first part will be allocated,
 * the second part will be inserted in the list.
 *
 * When we free a pointer, the corresponding memory block
 * will be found and freed.
 * If there are adjacent memory blocks in the list,
 * they will be collected.
 *
 * Complexity for memory allocation is O(N) and free is O(2N).
 *
 * \todo(vincent)
 * The base_addr might not be valid, which might cause
 * some issues. We will deal with it in the future.
 *
 *
 * An example usage:
 * ```
 * auto alct = new raintime::LinearAllocator(0, 128);
 *
 * auto p1 = alct->AllocRaw(32, 32);
 * auto p2 = alct->AllocRaw(32, 31);
 * auto p3 = alct->AllocRaw(32, 64);
 *
 * alct->FreeRaw(p1);
 * alct->FreeRaw(p2);
 * alct->FreeRaw(p3);
 * ```
 */
class LinearAllocator : public Allocator {
 public:
  /*! The memory block concept used within this allocator */
  struct MemBlk {
    struct MemBlk *prev; /*!< previous block */
    struct MemBlk *next; /*!< next block */
    size_t base_addr;    /*!< base address of this block */
    size_t num_bytes;    /*!< number of bytes inside this block */
    bool is_alloc;       /*!< whether this block is allocated */
  };

  /*!
   * The constructor of the linear allocator should specify the memory
   * space it required.
   * \todo(vincent) we need to verify these two input parameters
   * \todo(vincent) we should specify which type of memory it should allocate
   * \todo(vincent) a better approach is to use smart pointers
   *
   * \param base_addr Base address of the total memory space
   * \param num_bytes Size of the total memory space
   */
  LinearAllocator(size_t base_addr, size_t num_bytes) {
    // test whether the base_addr is aligned
    if ((base_addr & (this->kAllocAlign - 1)) ||
        (num_bytes & (this->kAllocAlign - 1)))
      throw std::invalid_argument(
          "base_addr and num_bytes should be aligned to kAllocAlign");

    this->base_addr = base_addr;
    this->num_bytes = num_bytes;

    // initialise the header
    // with the whole memory space
    this->head = new MemBlk;
    this->head->prev = nullptr;
    this->head->next = nullptr;
    this->head->base_addr = this->base_addr;
    this->head->num_bytes = this->num_bytes;
    this->head->is_alloc = false;
  }
  static LinearAllocator *Global(size_t base_addr, size_t num_bytes){
    static auto global = new raintime::LinearAllocator(base_addr, num_bytes);
    return global;
  }
  /*!
   * The destructor of the LinearAllocator.
   *
   * Iterate through every block and delete them.
   */
  ~LinearAllocator();

  // TODO(vincent) need to implement the allocation algorithm based on MemBlk
  void *AllocRaw(size_t align, size_t num_bytes);

  void FreeRaw(void *p);

  /*! Get the first block (head) in the list of memory blocks.
   * \return The head block
   */
  struct MemBlk *get_head() {
    return this->head;
  }

 private:
  size_t base_addr; /*!< base address of the memory block that
                       this allocator is
                       in charge of */
  size_t num_bytes; /*!< number of bytes within the whole memory block */

  struct MemBlk *head;
};
}

#endif
