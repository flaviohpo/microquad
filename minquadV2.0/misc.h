#ifndef __MISC_H
#define __MISC_H

#define constrain(val,min,max) ((val)<(min)?(min):(val)>(max)?(max):val) // limita um valor entre min e max

#define set_array(a,val,n) do { int ind = 0; do { a[ind] = val; ind++; } while(ind<n); } while(0)

#endif // __MISC_H
