#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/MathTools.h>

namespace chaos
{

/**
 * this code comes mostly from std::priority_queue 
 * this class has been designed to provide an additional 'remove' function
 * the algorithm is design around the hypothesis that the nodes are stored as heap,
 * and that the nodes are stored level by level
 *
 * example tree:
 *
 *                  100
 *         50                  80 
 *    25        30        77        78
 * 20   13   12   11   60
 *
 * the storage in the internal vector is
 *   level 0: 100
 *   level 1:  50
 *             80
 *   level 2:  25
 *             30
 *             77
 *             78
 *   level 3:  20 
 *             13
 *             12 
 *             11
 *             60
 *
 */

template<
    typename T, 
    typename _Pr = std::less<T>
>
class PriorityQueue
{

public:

    /** for STL compatibility */
    typedef std::vector<T> _Container;
    /** for STL compatibility */
    typedef typename _Container::value_type value_type;
    /** for STL compatibility */
    typedef typename _Container::size_type size_type;
    /** for STL compatibility */
    typedef typename _Container::reference reference;
    /** for STL compatibility */
    typedef typename _Container::const_reference const_reference;

    /** test if queue is empty */
    bool empty() const { return (c.empty()); }

    /** return length of queue */
    size_type size() const { return (c.size()); }

    /** return highest-priority element */
    const_reference top() const { return (c.cfront()); }

    /** return mutable highest-priority element (retained) */
    reference top() {  return (c.front()); }

    /** insert value in priority order */
    void insert(const value_type& _Val)
    {
        c.push_back(_Val);
        push_heap(c.begin(), c.end(), comp);
    }

    /** erase highest-priority element */
    void pop()
    {
        pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }

    /** remove an element by value */
    void remove(const value_type & _Val)
    {
        std::vector<T>::iterator it = std::find(c.begin(), c.end(), _Val);
        if (it != c.end())
        {
            size_type count = c.size();
            size_type index = it - c.begin();

            // copy the last element  to current position & remove last
            c[index] =  c[--count];
            c.pop_back();     
            if (index == count) // the last node has been removed. nothing to do
                return;
            if (count <= 1) // an empty tree, or with only a root is already heap-sorted
                return;
            update_indexed(index);
        }
    }

    /** the priority may have changed. Ensure container is still a heap by modifying the node   */
    /*  XXX : beware, this is a dangerous call because, we can change the priority of an object */
    /*        and the find call inside may access another element !!                            */
    /*  XXX : use with care, or use get_container instead                                       */
    void update(const value_type & _Val)
    {
        std::vector<T>::iterator it = std::find(c.begin(), c.end(), _Val);
        if (it != c.end())
        {
            size_type index = it - c.begin();
            update_indexed(index);
        }
    }

    /** a unity check method */
    bool test_heap(size_type index = 0) const
    {
        size_type level = chaos::MathTools::bsr(index + 1); 
        size_type base  = heap_baselevel(level); 
        size_type count = size();

        if (index >= count)
            return true;

        if (index > 0)
        {
            size_type parent_index = 
                (heap_baselevel(level - 1)) +    // the base of previous level
                (index - base) / 2;    

            if (!comp(c[index], c[parent_index]) && comp(c[parent_index], c[index]) ) // double test because of equality / inequality issue
                return false;
        }

        size_type first_child = heap_baselevel(level + 1) + (index - base) * 2;

        return test_heap(first_child) && test_heap(first_child + 1);
    }

    /** get internal container */
    _Container & get_container(){ return c;}

    /** get internal container */
    _Container const & get_container() const { return c;}

    /** an utility function used to displace a node whose key has changed */
    void update_indexed(size_type index)
    {
        size_type count = size();

        size_type level = chaos::MathTools::bsr(index + 1); // +1 : to ensure non 0 parameter. if index == 0 then level = 0 => OK

        // try to make node up, if it is too great
        // (in example, suppress 20 that is replaced by 60)
        while (index != 0)
        {
            size_type base = heap_baselevel(level); // the index of the first element of the same level

            size_type parent_index = 
                (heap_baselevel(level - 1)) +    // the base of previous level
                (index - base) / 2;              // due to storage consideration, the first_child of the 4th node of a level 
            // is the 8h node of next level
            if (comp(c[index], c[parent_index]))                    
                break;

            std::swap(c[index], c[parent_index]);
            index = parent_index;
            --level;
        }

        // try to make the node down, if it is too low
        // (in example, suppress the 80, that is replaced by 60)
        while (true) 
        {
            size_type base        = heap_baselevel(level); // the index of the first element of the same level
            size_type first_child = heap_baselevel(level + 1) + (index - base) * 2;  // due to storage, the first child of 2nd node of a 

            if (first_child < count)
            {
                if (first_child + 1 < count)  // take the greatest child
                {
                    if (comp(c[first_child], c[first_child + 1]))
                        first_child++;
                }

                if (comp(c[index], c[first_child]))
                {
                    std::swap(c[first_child], c[index]);
                    index = first_child;
                    ++level;
                    continue;
                }                  
            }
            break; // we want to process the loop until no their is a no-op step
        };
    }

protected:

    /** for a given level, returns the index of the first node of that level */
    static inline size_type heap_baselevel(size_type level)
    {
        return (1 << level) - 1;
    }

protected:

    /** the internal container */
    _Container c;
    /** the comparator functor */ 
    _Pr comp;
};

}; // namespace chaos

