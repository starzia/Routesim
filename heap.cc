/*Description:
A heap is a sequence of elements organized like a binary tree.
Each heap element corresponds to a tree node. The first value in
the sequence [first..last) is the root, and is the largest value
in the heap. Every element in the heap satisfies the following:
Every element is less than or equal to its parent. The largest
element is stored in the root, and all children hold progressively
smaller values. The make_heap function converts the range [first..last)
into a heap. The sort_heap function sorts a "heapified" sequence
that was created using the make_heap function. The push_heap function
inserts a new value into the heap. The pop_heap function swaps the
first and last elements in the heap specified by [first, last), then
reduces the length of the sequence by one before restoring the heap
property. The non-predicate versions of the heap functions use the
operator< for comparisons.
*/

//Note that you only need the pragma statement if you compile
//this on Windows.

#pragma warning(disable: 4786)

#include <iostream>
#include <algorithm>
#include <functional>
#include <vector>

using namespace std;


void main()
{
    const int VECTOR_SIZE = 8 ;

    // Define a template class vector of int
    typedef vector < int > IntVector ;

    //Define an iterator for template class vector of strings
    typedef IntVector::iterator IntVectorIt ;

    IntVector Numbers(VECTOR_SIZE) ;

    IntVectorIt it ;

    // Initialize vector Numbers
    Numbers[0] = 4 ;
    Numbers[1] = 10;
    Numbers[2] = 70 ;
    Numbers[3] = 10 ;
    Numbers[4] = 30 ;
    Numbers[5] = 69 ;
    Numbers[6] = 96 ;
    Numbers[7] = 100;

    // print content of Numbers
    cout << "Numbers { " ;
    for(it = Numbers.begin(); it != Numbers.end(); it++)
        cout << *it << " " ;
    cout << " }\n" << endl ;

    // convert Numbers into a heap
    make_heap(Numbers.begin(), Numbers.end()) ;

    cout << "After calling make_heap\n" << endl ;

    // print content of Numbers
    cout << "Numbers { " ;
    for(it = Numbers.begin(); it != Numbers.end(); it++)
        cout << *it << " " ;
    cout << " }\n" << endl ;

    // sort the heapified sequence Numbers
    sort_heap(Numbers.begin(), Numbers.end()) ;

    cout << "After calling sort_heap\n" << endl ;

    // print content of Numbers
    cout << "Numbers { " ;
    for(it = Numbers.begin(); it != Numbers.end(); it++)
        cout << *it << " " ;
    cout << " }\n" << endl ;

    //insert an element in the heap
    Numbers.push_back(7) ;
    push_heap(Numbers.begin(), Numbers.end()) ;

cout << "After calling push_heap but before make_heap\n" << endl ;


// print content of Numbers
    cout << "Numbers { " ;
    for(it = Numbers.begin(); it != Numbers.end(); it++)
        cout << *it << " " ;
    cout << " }\n" << endl ;


    // you need to call make_heap to re-assert the
    // heap property
 make_heap(Numbers.begin(), Numbers.end()) ;

    cout << "After calling make_heap\n" << endl ;

    // print content of Numbers
    cout << "Numbers { " ;
    for(it = Numbers.begin(); it != Numbers.end(); it++)
        cout << *it << " " ;
    cout << " }\n" << endl ;

    // remove the root element from the heap Numbers
    pop_heap(Numbers.begin(), Numbers.end()) ;

    cout << "After calling pop_heap\n" << endl ;

    // print content of Numbers
    cout << "Numbers { " ;
    for(it = Numbers.begin(); it != Numbers.end(); it++)
        cout << *it << " " ;
    cout << " }\n" << endl ;
}


