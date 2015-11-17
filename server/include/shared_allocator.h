#ifndef SHARED_ALLOCATOR_H
#define SHARED_ALLOCATOR_H

// ---- allocator for shared vector ----


template <typename T>
  class mmap_allocator {
  public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T const * const_pointer;
    typedef T& reference;
    typedef T const & const_reference;
    typedef T value_type;

    template <typename U>
    struct rebind {
      typedef mmap_allocator <U> other;
    };

    pointer address (reference value ) const {
      return &value;
    }

    const_pointer address (const_reference value) const {
      return &value;
    }

    mmap_allocator () throw () {}

    mmap_allocator ( mmap_allocator const  &) throw () {}

    template <typename U>
    mmap_allocator (mmap_allocator <U> const &) throw () {}

    ~mmap_allocator() throw () {}

    size_type max_size () const throw() {
      return std::numeric_limits <size_type>::max() / sizeof(T);
    }

    pointer allocate (size_type num, void *  hint = 0) {
      pointer p = (pointer) mmap(hint, num, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);
      int * val = (int *)p;
      if(val && *val == -1) p = NULL;
      return p;
    }

    void construct (pointer p, const_reference value) {
      new((void *)p) T(value);
    }

    void destroy (pointer p) {
      p->~T();
    }

    void deallocate (pointer p, size_type num) {
      munmap((caddr_t) p, num);
    }

  };

  template <typename T1, typename T2>
  bool operator == (mmap_allocator <T1> const &, mmap_allocator <T2> const &) throw () {
    return true;
  }

  template <typename T1, typename T2>
  bool operator != (mmap_allocator <T1> const &, mmap_allocator <T2> const &) throw () {
    return false;
  }


#endif // SHARED_ALLOCATOR_H
